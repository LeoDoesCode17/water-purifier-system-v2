#include "page_manager.h"
#include "ui/i_page.h"
#include "sensor_manager.h"
#include "ui/landing_page.h"
#include "ui/menu_page.h"
#include "ui/sub_page.h"
#include "ui/wifi_configuration_page.h"
#include "ui/config_mode_page.h"

namespace
{
    LiquidCrystal_I2C *lcd_ptr = nullptr;
    pages::IPage *current_page = nullptr;
    long last_encoder_position = 0;

    pages::IPage *page_for(pages::PageId id)
    {
        switch (id)
        {
        case pages::PageId::Landing:
            return &pages::landing_page;
        case pages::PageId::Menu:
            return &pages::menu_page;
        case pages::PageId::Sensors:
            return &pages::sensors_page;
        case pages::PageId::Control:
            return &pages::control_page;
        case pages::PageId::Network:
            return &pages::network_page;
        case pages::PageId::Settings:
            return &pages::settings_page;
        case pages::PageId::About:
            return &pages::about_page;
        case pages::PageId::WifiConfig:
            return &pages::wifi_configuration_page;
        case pages::PageId::ConfigMode:
            return &pages::config_mode_page;
        }
        return &pages::landing_page;
    }
}

namespace page_manager
{
    void initialize(LiquidCrystal_I2C &lcd)
    {
        lcd_ptr = &lcd;
        navigate_to(pages::PageId::Landing);
    }

    void update()
    {
        if (current_page == nullptr || lcd_ptr == nullptr)
        {
            return;
        }

        const long position = sensor_manager::getEncoderPosition();
        const long delta = position - last_encoder_position;
        last_encoder_position = position;
        const bool button_pressed = sensor_manager::wasEncoderButtonPressed();
        current_page->update(*lcd_ptr, delta, button_pressed);
    }

    void navigate_to(pages::PageId id)
    {
        if (current_page != nullptr)
        {
            current_page->onExit();
        }

        sensor_manager::setEncoderPosition(0);
        last_encoder_position = 0;
        current_page = page_for(id);
        current_page->onEnter(*lcd_ptr);
    }

} // namespace page_manager
