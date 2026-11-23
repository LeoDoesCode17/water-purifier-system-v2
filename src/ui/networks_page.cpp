#include "networks_page.h"
#include "managers/page_manager.h"
#include "page_id.h"

namespace pages
{
    NetworksPage networks_page;

    namespace
    {
        constexpr int LCD_COLS = 20;
        constexpr int LCD_ROWS = 4;
        constexpr int TITLE_ROWS = 1;                        // Row 0 is reserved for the title
        constexpr int VISIBLE_ITEMS = LCD_ROWS - TITLE_ROWS; // Rows 1-3 show menu items (3 at a time)
        const char *const TITLE = "NETWORKS";

        struct NetworkItem
        {
            const char *label;
            PageId target;
        };

        const NetworkItem ITEMS[] = {
            {"Configure", PageId::NetworkConfig},
            {"Status", PageId::NetworkStatus},
            {"Back", PageId::Settings},
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

    void NetworksPage::onEnter(LiquidCrystal_I2C &lcd)
    {
        lcd.clear();
        lcd.backlight();

        // Draw the title once. render() never touches row 0, so the title
        // stays in place no matter how the list scrolls.
        lcd.setCursor(0, 0);
        printPadded(lcd, TITLE, LCD_COLS);

        // selected_ and top_ are kept, so coming back from an item page
        // leaves the cursor on the item you just opened.
        render(lcd);
    }

    void NetworksPage::onExit() {}

    void NetworksPage::update(LiquidCrystal_I2C &lcd, long encoderDelta, bool buttonPressed)
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

            // Scroll the visible window so the cursor stays on screen.
            // The window is now VISIBLE_ITEMS (3) items tall, not 4.
            if (selected_ < top_)
            {
                top_ = selected_;
            }
            else if (selected_ >= top_ + VISIBLE_ITEMS)
            {
                top_ = selected_ - (VISIBLE_ITEMS - 1);
            }

            render(lcd);
        }
    }

    void NetworksPage::render(LiquidCrystal_I2C &lcd)
    {
        // Only draws the item area (LCD rows 1-3); row 0 (title) is left alone.
        for (int row = 0; row < VISIBLE_ITEMS; row++)
        {
            const int index = top_ + row;       // Which menu item belongs on this row
            lcd.setCursor(0, row + TITLE_ROWS); // Shift down by one row to skip the title

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