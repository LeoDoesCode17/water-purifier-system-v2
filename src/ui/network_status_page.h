#pragma once
#include <Arduino.h>
#include <IPAddress.h>
#include "i_page.h"

namespace pages
{
    class NetworkStatusPage : public IPage
    {
    public:
        void onEnter(LiquidCrystal_I2C &lcd) override;
        void onExit() override;
        void update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed) override;

    private:
        String lineText(int index);                 // Text of list entry `index`, built from the cached state
        void scrollIntoView();                      // Moves the window so the cursor stays visible
        void render(LiquidCrystal_I2C &lcd);        // Draws the 3 visible rows (LCD rows 1-3)

        int selected_ = 0;                          // Index of the entry under the cursor
        int top_ = 0;                               // Index of the entry shown on LCD row 1

        // Last values read from network_manager (what is currently drawn)
        bool wifi_ = false;
        bool mqtt_ = false;
        IPAddress ip_;                              // Defaults to 0.0.0.0
        unsigned long lastPollMs_ = 0;              // When the status was last read
    };

    extern NetworkStatusPage network_status_page;
}