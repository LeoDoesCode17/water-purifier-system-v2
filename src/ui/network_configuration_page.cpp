#include "network_configuration_page.h"
#include "lcd_utils.h"
#include "page_id.h"
#include "managers/page_manager.h"
#include "managers/network_manager.h"

namespace pages
{
    NetworkConfigurationPage network_configuration_page; // Create the global instance

    namespace
    {
        constexpr int LCD_ROWS = 4;     // Visible rows on the 20x4 LCD
        constexpr int CURSOR_WIDTH = 2; // "> " or "  " in front of every line
        constexpr int VALUE_INDENT = 1; // Values are indented by one space under their label
        constexpr int SCROLL_COL = 19;  // Last column: shows the ^ / v "more content" markers
        // Characters left for a value on one row: 19 - 2 - 1 = 16
        constexpr int VALUE_WIDTH = SCROLL_COL - CURSOR_WIDTH - VALUE_INDENT;
    }

    void NetworkConfigurationPage ::onEnter(LiquidCrystal_I2C &lcd)
    {
        apError_ = false; // Forget any error from a previous visit
        selected_ = 0;    // Start at the top so the saved values are visible first
        top_ = 0;
        buildLines(); // Read the current config and build the content

        lcd.clear();     // Wipe the previous page
        lcd.backlight(); // Make sure the backlight is on
        render(lcd);     // Draw the first 4 lines
    }

    void NetworkConfigurationPage ::onExit() {} // Nothing to clean up

    void NetworkConfigurationPage ::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        if (buttonPressed) // Encoder button was clicked
        {
            switch (lines_[selected_].action) // Decide by the line under the cursor
            {
            case Action::Configure:
                if (network_manager::start_ap()) // Try to start AP + web server
                {
                    page_manager::navigate_to(PageId::ConfigMode); // Success: show the guide page
                }
                else
                {
                    apError_ = true; // Failure: add an error line on top of the content
                    buildLines();    // Rebuild the list (now including the error line)
                    selected_ = 0;   // Jump to the top so the error is visible
                    top_ = 0;
                    render(lcd);
                }
                break;

            case Action::Back:
                page_manager::navigate_to(PageId::Networks); // Return to the menu
                break;

            case Action::None: // Cursor is on an info line (SSID, host...): nothing to do
                break;
            }
            return; // Done for this loop iteration
        }

        if (encoderDelta != 0) // Knob was rotated
        {
            const int last = static_cast<int>(lines_.size()) - 1; // Index of the last line
            // Clockwise scrolls down. Clamped (no wrap-around) because this is a content page,
            // not a menu. Flip the sign of the delta if the direction feels inverted.
            const int next = constrain(selected_ + static_cast<int>(encoderDelta), 0, last);

            if (next != selected_) // Only redraw if the cursor really moved
            {
                selected_ = next;
                scrollIntoView(); // Move the window if the cursor left it
                render(lcd);
            }
        }
    }

    void NetworkConfigurationPage ::buildLines()
    {
        lines_.clear(); // Start from an empty list

        // Read what the web form has delivered (empty strings if nothing was configured yet)
        const auto wifi = network_manager::getWifiConfig();
        const auto mqtt = network_manager::getMqttConfig();

        if (apError_)
        {
            addLine("AP start failed!", Action::None); // Shown only after a failed Configure
        }

        addLine("Wi-Fi SSID:", Action::None);    // Label
        addValue(wifi.ssid);                     // Value (wrapped if long)
        addLine("MQTT Host:", Action::None);     // Label
        addValue(mqtt.host);                     // Value (wrapped if long)
        addLine("Configure", Action::Configure); // Button 1
        addLine("Back", Action::Back);           // Button 2
    }

    void NetworkConfigurationPage ::addLine(const String &text, Action action)
    {
        lines_.push_back(Line{text, action}); // Append one line to the content
    }

    void NetworkConfigurationPage ::addValue(const String &value)
    {
        if (value.length() == 0) // Nothing received yet
        {
            addLine(" Not configured", Action::None);
            return;
        }

        // Cut the value into pieces of VALUE_WIDTH characters, one piece per line
        for (unsigned int pos = 0; pos < value.length(); pos += VALUE_WIDTH)
        {
            addLine(" " + value.substring(pos, pos + VALUE_WIDTH), Action::None); // Leading space = indent
        }
    }

    void NetworkConfigurationPage ::scrollIntoView()
    {
        if (selected_ < top_) // Cursor moved above the window: scroll up
        {
            top_ = selected_;
        }
        else if (selected_ >= top_ + LCD_ROWS) // Cursor moved below the window: scroll down
        {
            top_ = selected_ - (LCD_ROWS - 1);
        }
    }

    void NetworkConfigurationPage ::render(LiquidCrystal_I2C &lcd)
    {
        const int count = static_cast<int>(lines_.size());

        for (int row = 0; row < LCD_ROWS; row++)
        {
            const int index = top_ + row; // Which content line belongs on this LCD row

            if (index < count)
            {
                lcd_utils::printLine(lcd, row, String(index == selected_ ? "> " : "  ") + lines_[index].text);
            }
            else
            {
                lcd_utils::printLine(lcd, row, ""); // Past the end of the content: blank row
            }
        }

        // "More content" markers in the last column (printLine already cleared it to a space)
        if (top_ > 0)
        {
            lcd.setCursor(SCROLL_COL, 0);
            lcd.print('^'); // Something above the window
        }
        if (top_ + LCD_ROWS < count)
        {
            lcd.setCursor(SCROLL_COL, LCD_ROWS - 1);
            lcd.print('v'); // Something below the window
        }
    }
}