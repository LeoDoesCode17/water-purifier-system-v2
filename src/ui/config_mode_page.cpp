#include "config_mode_page.h"
#include "lcd_utils.h"
#include "page_id.h"
#include "managers/page_manager.h"
#include "managers/network_manager.h"

namespace pages
{
    ConfigModePage config_mode_page;

    void ConfigModePage::onEnter(LiquidCrystal_I2C &lcd)
    {
        showingSaved_ = false; // Fresh session: show the guide first
        lcd.clear();
        lcd.backlight();

        // The guide: join the AP, then open the IP in a browser
        lcd_utils::printLine(lcd, 0, "AP: " + network_manager::getApSsid());
        lcd_utils::printLine(lcd, 1, "Pass: " + network_manager::getApPassword());
        lcd_utils::printLine(lcd, 2, "Open: " + network_manager::getApIp());
        lcd_utils::printLine(lcd, 3, "> Cancel"); // The only button, always selected
    }

    void ConfigModePage::onExit()
    {
        network_manager::stop_ap(); // Leaving this page for ANY reason shuts down AP + web server
    }

    void ConfigModePage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        // The web server (running in loop()) sets this flag when the form is submitted
        if (!showingSaved_ && network_manager::hasSubmittedCredentials())
        {
            showingSaved_ = true; // Only switch screens once
            lcd_utils::printLine(lcd, 0, "Credentials saved!");
            lcd_utils::printLine(lcd, 1, "Check Serial Monitor");
            lcd_utils::printLine(lcd, 2, "");
            lcd_utils::printLine(lcd, 3, "> Back"); // Same button, new label
        }

        if (buttonPressed) // Cancel (or Back after saving)
        {
            page_manager::navigate_to(PageId::Networks); // onExit() above will stop the AP
        }
    }
}