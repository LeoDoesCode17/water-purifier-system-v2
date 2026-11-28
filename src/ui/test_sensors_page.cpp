#include "test_sensors_page.h"
#include "lcd_utils.h"
#include "page_id.h"
#include "core/type.h"
#include "managers/page_manager.h"
#include "managers/sensor_manager.h"

namespace pages
{
    TestSensorsPage test_sensors_page;

    namespace
    {
        constexpr int LCD_COLS = lcd_utils::COLS;
        constexpr int LCD_ROWS = 4;
        constexpr int TITLE_ROWS = 1;                        // Row 0 is reserved for the title
        constexpr int VISIBLE_ITEMS = LCD_ROWS - TITLE_ROWS; // Rows 1-3 show list items (3 at a time)
        constexpr unsigned long POLL_INTERVAL_MS = 500;      // Sensor data is re-read twice per second
        const char *const TITLE = "TEST SENSORS";

        // Where "Back" goes (same target as your test actuators page)
        const PageId BACK_TARGET = PageId::Settings;

        // The list entries, in display order: one per uart_sensor_t field, then Back
        enum Item
        {
            ITEM_PH,
            ITEM_TDS,
            ITEM_TURBIDITY,
            ITEM_DISTANCE,
            ITEM_TEMPERATURE,
            ITEM_BACK, // Always last. Its value also equals the number of sensor entries.
        };
        constexpr int ITEM_COUNT = ITEM_BACK + 1;

        // Row labels for the sensor entries (the ph/tds/turbidity fields are raw voltages)
        const char *const LABELS[ITEM_BACK] = {"pH Volt", "TDS Volt", "Turb Volt", "Distance", "Temp"};

        // The value text currently drawn for each sensor entry, e.g. "2.512 V".
        // Comparing text (not raw floats) means we only redraw when the displayed digits change,
        // so tiny noise below the displayed precision doesn't cause redraws.
        String shown_value[ITEM_BACK];
        unsigned long last_poll_ms = 0;

        // Formats one field of the sensor struct into its display text
        String valueText(const uart_sensor_t &data, int item)
        {
            switch (item)
            {
            case ITEM_PH:
                return String(data.ph_voltage, 3) + " V"; // 3 decimals, e.g. "2.512 V"
            case ITEM_TDS:
                return String(data.tds_voltage, 3) + " V";
            case ITEM_TURBIDITY:
                return String(data.turbidity_voltage, 3) + " V";
            case ITEM_DISTANCE:
                return String(data.distance_cm, 1) + " cm"; // 1 decimal, e.g. "23.4 cm"
            case ITEM_TEMPERATURE:
                return String(data.temperature_celcius, 1) + " C"; // e.g. "27.3 C"
            }
            return "";
        }

        // Reads the latest sensor data and updates shown_value[].
        // Returns true if any displayed text changed (i.e. the screen needs a redraw).
        bool refreshValues()
        {
            const uart_sensor_t data = sensor_manager::get_uart_sensor_data();
            bool changed = false;

            for (int i = 0; i < ITEM_BACK; i++)
            {
                const String text = valueText(data, i);
                if (text != shown_value[i])
                {
                    shown_value[i] = text;
                    changed = true;
                }
            }
            return changed;
        }

        // Builds one 20-column row: cursor + label on the left, value right-aligned.
        // Example: "> pH Volt        2.512 V". Pass an empty value for rows without one (Back).
        String makeRow(bool selected, const char *label, const String &value)
        {
            String row = String(selected ? "> " : "  ") + label;

            if (value.length() > 0)
            {
                row += ' '; // At least one space between label and value
                while (static_cast<int>(row.length()) < LCD_COLS - static_cast<int>(value.length()))
                {
                    row += ' '; // Spaces push the value to the right edge
                }
                row += value;
            }
            return row;
        }
    }

    void TestSensorsPage::onEnter(LiquidCrystal_I2C &lcd)
    {
        lcd.clear();
        lcd.backlight();

        // Draw the title once. render() never touches row 0, so it stays put while scrolling.
        lcd_utils::printLine(lcd, 0, TITLE);

        // selected_ and top_ are kept between visits, so the cursor stays where you left it.
        refreshValues();         // Fill shown_value[] with the current readings
        last_poll_ms = millis(); // Start the poll timer from now
        render(lcd);
    }

    void TestSensorsPage::onExit() {}

    void TestSensorsPage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        if (buttonPressed)
        {
            if (selected_ == ITEM_BACK) // Only "Back" does something; sensor rows are info only
            {
                page_manager::navigate_to(BACK_TARGET);
            }
            return;
        }

        if (encoderDelta != 0) // Knob was rotated: move the cursor
        {
            // Wrap around at both ends (flip the sign if the direction feels inverted)
            const int step = static_cast<int>(encoderDelta % ITEM_COUNT);
            selected_ = (selected_ + step + ITEM_COUNT) % ITEM_COUNT;

            // Scroll the visible window so the cursor stays on screen
            if (selected_ < top_)
            {
                top_ = selected_;
            }
            else if (selected_ >= top_ + VISIBLE_ITEMS)
            {
                top_ = selected_ - (VISIBLE_ITEMS - 1);
            }

            render(lcd);
        }

        // The UART task keeps delivering new readings, so refresh the values periodically.
        // Throttled so we don't re-read and re-format on every loop() iteration.
        if (millis() - last_poll_ms >= POLL_INTERVAL_MS)
        {
            last_poll_ms = millis();
            if (refreshValues())
            {
                render(lcd);
            }
        }
    }

    void TestSensorsPage::render(LiquidCrystal_I2C &lcd)
    {
        // Only draws the list area (LCD rows 1-3); row 0 (title) is left alone.
        for (int row = 0; row < VISIBLE_ITEMS; row++)
        {
            const int index = top_ + row; // Which list entry belongs on this row
            const bool selected = (index == selected_);
            String text;

            if (index < ITEM_BACK)
            {
                text = makeRow(selected, LABELS[index], shown_value[index]);
            }
            else if (index == ITEM_BACK)
            {
                text = makeRow(selected, "Back", "");
            }
            // else: past the end of the list, text stays empty (blank row)

            lcd_utils::printLine(lcd, row + TITLE_ROWS, text);
        }
    }
}