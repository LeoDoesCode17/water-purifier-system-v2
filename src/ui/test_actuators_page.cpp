#include "test_actuators_page.h"
#include "lcd_utils.h"
#include "page_id.h"
#include "managers/page_manager.h"
#include "managers/actuator_manager.h"

namespace pages
{
    TestActuatorsPage test_actuators_page;

    namespace
    {
        // Lets us write `Actuator::VALVE` whether the enum lives in actuator_manager
        // or in a global header such as core/type.h.
        using namespace actuator_manager;

        constexpr int LCD_COLS = lcd_utils::COLS;
        constexpr int LCD_ROWS = 4;
        constexpr int TITLE_ROWS = 1;                        // Row 0 is reserved for the title
        constexpr int VISIBLE_ITEMS = LCD_ROWS - TITLE_ROWS; // Rows 1-3 show list items (3 at a time)
        const char *const TITLE = "TEST ACTUATORS";

        // Where "Back" goes. Your old file used Settings for every item, so I kept that.
        const PageId BACK_TARGET = PageId::Settings;

        // true: every actuator is switched off when you leave this page, so a test can't
        // leave a pump or motor running by accident. Set to false to keep them as they are.
        constexpr bool TURN_OFF_ON_EXIT = true;

        // The actuators that can be tested, in display order
        struct ActuatorItem
        {
            const char *label;
            Actuator actuator;
        };

        const ActuatorItem ACTUATORS[] = {
            {"Valve", Actuator::VALVE},
            {"Pump 1", Actuator::PUMP_1},
            {"Pump 2", Actuator::PUMP_2},
            {"Motor Gear", Actuator::MOTOR_GEAR},
        };
        constexpr int ACTUATOR_COUNT = sizeof(ACTUATORS) / sizeof(ACTUATORS[0]);
        constexpr int BACK_INDEX = ACTUATOR_COUNT;     // "Back" is the entry right after the actuators
        constexpr int ITEM_COUNT = ACTUATOR_COUNT + 1; // Actuators + Back

        // What is currently drawn for each actuator (true = ON). Compared with the real
        // state in update() so the page redraws if something else switches an actuator.
        bool shown_state[ACTUATOR_COUNT] = {};

        // Re-read the real state of every actuator from the manager
        void readStates()
        {
            for (int i = 0; i < ACTUATOR_COUNT; i++)
            {
                shown_state[i] = actuator_manager::is_actuator_on(ACTUATORS[i].actuator);
            }
        }

        // Builds one 20-column row: cursor + label on the left, status right-aligned.
        // Example: "> Pump 1            OFF". Pass status = nullptr for rows without status.
        String makeRow(bool selected, const char *label, const char *status)
        {
            String row = String(selected ? "> " : "  ") + label;

            if (status != nullptr)
            {
                const int statusLength = strlen(status);
                while (static_cast<int>(row.length()) < LCD_COLS - statusLength)
                {
                    row += ' '; // Spaces push the status to the right edge
                }
                row += status;
            }
            return row;
        }
    }

    void TestActuatorsPage::onEnter(LiquidCrystal_I2C &lcd)
    {
        lcd.clear();
        lcd.backlight();

        // Draw the title once. render() never touches row 0, so it stays put while scrolling.
        lcd_utils::printLine(lcd, 0, TITLE);

        // selected_ and top_ are kept between visits, so the cursor stays where you left it.
        readStates(); // Show the real state of every actuator
        render(lcd);
    }

    void TestActuatorsPage::onExit()
    {
        if (TURN_OFF_ON_EXIT)
        {
            for (int i = 0; i < ACTUATOR_COUNT; i++)
            {
                if (actuator_manager::is_actuator_on(ACTUATORS[i].actuator))
                {
                    actuator_manager::turn_off(ACTUATORS[i].actuator);
                }
            }
        }
    }

    void TestActuatorsPage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        if (buttonPressed)
        {
            if (selected_ == BACK_INDEX)
            {
                page_manager::navigate_to(BACK_TARGET); // onExit() runs here and switches everything off
            }
            else
            {
                // An actuator is selected: toggle it
                const Actuator actuator = ACTUATORS[selected_].actuator;

                if (actuator_manager::is_actuator_on(actuator))
                {
                    actuator_manager::turn_off(actuator);
                }
                else
                {
                    actuator_manager::turn_on(actuator);
                }

                shown_state[selected_] = actuator_manager::is_actuator_on(actuator); // Read back what really happened
                render(lcd);
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

        // Something else (another task, MQTT command...) may have switched an actuator
        // while this page is open. Reading these flags is cheap, so check on every loop.
        bool changed = false;
        for (int i = 0; i < ACTUATOR_COUNT; i++)
        {
            const bool now = actuator_manager::is_actuator_on(ACTUATORS[i].actuator);
            if (now != shown_state[i])
            {
                shown_state[i] = now;
                changed = true;
            }
        }
        if (changed)
        {
            render(lcd);
        }
    }

    void TestActuatorsPage::render(LiquidCrystal_I2C &lcd)
    {
        // Only draws the list area (LCD rows 1-3); row 0 (title) is left alone.
        for (int row = 0; row < VISIBLE_ITEMS; row++)
        {
            const int index = top_ + row; // Which list entry belongs on this row
            const bool selected = (index == selected_);
            String text;

            if (index < ACTUATOR_COUNT)
            {
                text = makeRow(selected, ACTUATORS[index].label, shown_state[index] ? "ON" : "OFF");
            }
            else if (index == BACK_INDEX)
            {
                text = makeRow(selected, "Back", nullptr);
            }
            // else: past the end of the list, text stays empty (blank row)

            lcd_utils::printLine(lcd, row + TITLE_ROWS, text);
        }
    }
}