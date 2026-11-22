#include "sub_page.h"
#include "managers/page_manager.h"
#include "page_id.h"

namespace pages
{
    SubPage sensors_page("SENSORS");
    SubPage control_page("CONTROL");
    SubPage network_page("NETWORK");
    SubPage settings_page("SETTINGS");
    SubPage about_page("ABOUT");

    void SubPage::onEnter(LiquidCrystal_I2C &lcd)
    {
        lcd.clear();
        lcd.backlight();

        lcd.setCursor(0, 0);
        lcd.print(title_);

        // TODO: draw this page's real content on rows 1-2

        // Back button on the last row, always selected (only one option)
        lcd.setCursor(0, 3);
        lcd.print("> Back");
    }

    void SubPage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        if (buttonPressed)
        {
            page_manager::navigate_to(PageId::Menu);
        }
    }
}