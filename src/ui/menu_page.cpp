#include "menu_page.h"
#include "managers/page_manager.h"
#include "page_id.h"

namespace pages
{
    MenuPage menu_page;

    namespace
    {
        constexpr int LCD_COLS = 20;
        constexpr int LCD_ROWS = 4;

        struct MenuItem
        {
            const char *label;
            PageId target;
        };

        const MenuItem ITEMS[] = {
            {"Sensors", PageId::Sensors},
            {"Control", PageId::Control},
            {"Network", PageId::Network},
            {"Wifi Config", PageId::WifiConfig}, // new
            {"Settings", PageId::Settings},
            {"About", PageId::About},
        };
        constexpr int ITEM_COUNT = sizeof(ITEMS) / sizeof(ITEMS[0]);

        // Print text and pad with spaces up to `width` so old characters
        // get overwritten without needing lcd.clear() (avoids flicker).
        void printPadded(LiquidCrystal_I2C &lcd, const char *text, int width)
        {
            int written = 0;
            while (text[written] != '\0' && written < width)
            {
                lcd.print(text[written]);
                written++;
            }
            while (written < width)
            {
                lcd.print(' ');
                written++;
            }
        }
    }

    void MenuPage::onEnter(LiquidCrystal_I2C &lcd)
    {
        lcd.clear();
        lcd.backlight();
        // selected_ and top_ are kept, so coming back from an item page
        // leaves the cursor on the item you just opened.
        render(lcd);
    }

    void MenuPage::onExit() {}

    void MenuPage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
    {
        if (buttonPressed)
        {
            page_manager::navigate_to(ITEMS[selected_].target);
            return;
        }

        if (encoderDelta != 0)
        {
            // Wrap around at both ends. If the direction feels inverted,
            // change `+` to `-` on the next line.
            const int step = static_cast<int>(encoderDelta % ITEM_COUNT);
            selected_ = (selected_ + step + ITEM_COUNT) % ITEM_COUNT;

            // Scroll the visible window so the cursor stays on screen
            if (selected_ < top_)
            {
                top_ = selected_;
            }
            else if (selected_ >= top_ + LCD_ROWS)
            {
                top_ = selected_ - (LCD_ROWS - 1);
            }

            render(lcd);
        }
    }

    void MenuPage::render(LiquidCrystal_I2C &lcd)
    {
        for (int row = 0; row < LCD_ROWS; row++)
        {
            const int index = top_ + row;
            lcd.setCursor(0, row);

            if (index < ITEM_COUNT)
            {
                lcd.print(index == selected_ ? '>' : ' ');
                lcd.print(' ');
                printPadded(lcd, ITEMS[index].label, LCD_COLS - 2);
            }
            else
            {
                printPadded(lcd, "", LCD_COLS);
            }
        }
    }
}