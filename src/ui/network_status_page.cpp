#include "network_status_page.h"
#include "lcd_utils.h"
#include "page_id.h"
#include "managers/page_manager.h"
#include "managers/network_manager.h"

namespace pages
{
    NetworkStatusPage network_status_page;

    namespace
    {
        constexpr int LCD_ROWS = 4;
        constexpr int TITLE_ROWS = 1;                        // Row 0 is reserved for the title
        constexpr int VISIBLE_ITEMS = LCD_ROWS - TITLE_ROWS; // Rows 1-3 show list entries (3 at a time)
        constexpr int TEXT_WIDTH = lcd_utils::COLS - 2;      // 18 characters left after the "> " cursor
        constexpr unsigned long POLL_INTERVAL_MS = 500;      // Status is re-read twice per second

        const char *const TITLE = "NETWORK STATUS";

        // The list entries, in display order
        enum Item
        {
            ITEM_WIFI,
            ITEM_MQTT,
            ITEM_IP,
            ITEM_BACK,
        };
        constexpr int ITEM_COUNT = ITEM_BACK + 1;
    }

    void NetworkStatusPage::onEnter(LiquidCrystal_I2C &lcd)
    {
        lcd.clear();
        lcd.backlight();

        // Draw the title once. render() never touches row 0, so it stays put while scrolling.
        lcd_utils::printLine(lcd, 0, TITLE);

        selected_ = 0; // Start at the top so the Wi-Fi status is visible first
        top_ = 0;

        wifi_ = network_manager::isWifiConnected(); // Read the current state...
        mqtt_ = network_manager::isMqttConnected();
        ip_ = network_manager::getLocalIp();
        lastPollMs_ = millis(); // ...and start the poll timer from now

        render(lcd);
    }

    void NetworkStatusPage::onExit() {}

    void NetworkStatusPage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        if (buttonPressed)
        {
            if (selected_ == ITEM_BACK) // Only "Back" does something; the status lines are info only
            {
                page_manager::navigate_to(PageId::Networks);
            }
            return;
        }

        if (encoderDelta != 0) // Knob was rotated: move the cursor
        {
            // Wrap around at both ends (flip the sign if the direction feels inverted)
            const int step = static_cast<int>(encoderDelta % ITEM_COUNT);
            selected_ = (selected_ + step + ITEM_COUNT) % ITEM_COUNT;
            scrollIntoView();
            render(lcd);
        }

        // Wi-Fi/MQTT can connect or drop while this page is open, so re-check periodically.
        // Throttled so we don't query the network manager on every loop() iteration.
        if (millis() - lastPollMs_ >= POLL_INTERVAL_MS)
        {
            lastPollMs_ = millis();

            const bool wifi = network_manager::isWifiConnected();
            const bool mqtt = network_manager::isMqttConnected();
            const IPAddress ip = network_manager::getLocalIp();

            if (wifi != wifi_ || mqtt != mqtt_ || !(ip == ip_)) // Redraw only if something changed
            {
                wifi_ = wifi;
                mqtt_ = mqtt;
                ip_ = ip;
                render(lcd);
            }
        }
    }

    String NetworkStatusPage::lineText(int index)
    {
        switch (index)
        {
        case ITEM_WIFI:
            return String("WiFi: ") + (wifi_ ? "Connected" : "Disconnected"); // max 18 characters

        case ITEM_MQTT:
            return String("MQTT: ") + (mqtt_ ? "Connected" : "Disconnected"); // max 18 characters

        case ITEM_IP:
        {
            if (!wifi_)
            {
                return "IP: -"; // No Wi-Fi, so no address to show (getLocalIp() would be 0.0.0.0)
            }
            const String address = ip_.toString();
            // "IP: " + a 15-character address (e.g. 192.168.100.100) would be 19 characters,
            // one too many. In that case drop the space after the colon.
            return (4 + static_cast<int>(address.length()) <= TEXT_WIDTH) ? "IP: " + address
                                                                          : "IP:" + address;
        }

        case ITEM_BACK:
            return "Back";
        }
        return "";
    }

    void NetworkStatusPage::scrollIntoView()
    {
        if (selected_ < top_) // Cursor moved above the window: scroll up
        {
            top_ = selected_;
        }
        else if (selected_ >= top_ + VISIBLE_ITEMS) // Cursor moved below the window: scroll down
        {
            top_ = selected_ - (VISIBLE_ITEMS - 1);
        }
    }

    void NetworkStatusPage::render(LiquidCrystal_I2C &lcd)
    {
        // Only draws the list area (LCD rows 1-3); row 0 (title) is left alone.
        for (int row = 0; row < VISIBLE_ITEMS; row++)
        {
            const int index = top_ + row; // Which list entry belongs on this row

            if (index < ITEM_COUNT)
            {   
                lcd_utils::printLine(lcd, row + TITLE_ROWS,
                                     String(index == selected_ ? "> " : "  ") + lineText(index));
            }
            else
            {
                lcd_utils::printLine(lcd, row + TITLE_ROWS, ""); // Past the end of the list: blank row
            }
        }
    }
}