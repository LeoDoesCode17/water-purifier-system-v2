#pragma once
#include <Arduino.h> // String
#include <vector>    // std::vector holds the list of lines
#include "i_page.h"

namespace pages
{
    class WifiConfigurationPage : public IPage
    {
    public:
        void onEnter(LiquidCrystal_I2C &lcd) override;
        void onExit() override;
        void update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed) override;

    private:
        // What pressing the encoder button does when the cursor is on a line
        enum class Action
        {
            None,      // Info line: pressing does nothing
            Configure, // Start configuration mode
            Back       // Return to the menu
        };

        // One line of the scrollable content
        struct Line
        {
            String text;
            Action action;
        };

        void buildLines();                               // Builds the full list of lines
        void addLine(const String &text, Action action); // Appends one line to the list
        void addValue(const String &value);              // Appends a value, wrapped over several lines
        void scrollIntoView();                           // Moves the window so the cursor is visible
        void render(LiquidCrystal_I2C &lcd);             // Draws the 4 visible rows

        std::vector<Line> lines_; // The whole page content (can be longer than 4 rows)
        int selected_ = 0;        // Index of the line under the cursor
        int top_ = 0;             // Index of the line shown on the first LCD row
        bool apError_ = false;    // true after "Configure" failed to start the access point
    };

    extern WifiConfigurationPage wifi_configuration_page; // The single instance used by PageManager
}