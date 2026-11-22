#pragma once
#include "i_page.h"

namespace pages
{
    class ConfigModePage : public IPage
    {
    public:
        void onEnter(LiquidCrystal_I2C &lcd) override;
        void onExit() override;
        void update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed) override;

    private:
        bool showingSaved_ = false; // true once we switched to the "saved" screen
    };

    extern ConfigModePage config_mode_page;
}