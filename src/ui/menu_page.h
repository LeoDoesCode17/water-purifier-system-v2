#pragma once
#include "i_page.h"

namespace pages
{
    class MenuPage : public IPage
    {
    public:
        void onEnter(LiquidCrystal_I2C &lcd) override;
        void onExit() override;
        void update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed) override;

    private:
        void render(LiquidCrystal_I2C &lcd);

        int selected_ = 0; // index of the item under the cursor
        int top_ = 0;      // index of the item shown on the first row
    };

    extern MenuPage menu_page;
}