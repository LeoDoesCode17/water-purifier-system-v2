#pragma once
#include "i_page.h"

namespace pages
{
    class LandingPage : public IPage
    {
        public:
            void onEnter(LiquidCrystal_I2C &lcd) override;
            void onExit() override;
            void update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed) override;
    };

    extern LandingPage landing_page;
}