#pragma once
#include "i_page.h"

namespace pages
{
    // A page with a title and a "Back" button. Pressing the encoder
    // button returns to the menu. Each menu item gets its own instance.
    class SubPage : public IPage
    {
    public:
        explicit SubPage(const char *title) : title_(title) {}

        void onEnter(LiquidCrystal_I2C &lcd) override;
        void onExit() override {}
        void update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed) override;

    private:
        const char *title_;
    };

    extern SubPage sensors_page;
    extern SubPage control_page;
    extern SubPage network_page;
    extern SubPage settings_page;
    extern SubPage about_page;
}