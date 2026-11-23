#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

namespace lcd_utils
{
    constexpr uint8_t COLS = 20; // Width of the LCD

    // Print `text` on `row`, padded with spaces to the full width.
    // The padding erases old characters, so no lcd.clear() (and no flicker).
    inline void printLine(LiquidCrystal_I2C &lcd, uint8_t row, const String &text)
    {
        lcd.setCursor(0, row);             // Start of the row
        for (uint8_t i = 0; i < COLS; i++) // Exactly 20 characters get written
        {
            lcd.print(i < text.length() ? text[i] : ' '); // Real character, or a space after the text ends
        }
    }
}