#include "landing_page.h"
#include "managers/page_manager.h"
#include "page_id.h"

namespace pages
{
    LandingPage landing_page;

    namespace
    {
    }

    void LandingPage::onEnter(LiquidCrystal_I2C &lcd)
    {
        // Turn on the LED backlight
        lcd.clear();
        lcd.backlight();

        // Row 0: Column 0 to 19
        lcd.setCursor(0, 0);
        lcd.print("LANDING PAGE");

        // Row 1: Column 0 to 19
        lcd.setCursor(0, 1);
        lcd.print("Press button");
    }
    void LandingPage::onExit() {}
    void LandingPage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        if (buttonPressed)
        {
            page_manager::navigate_to(PageId::Menu);
        }
    }
}