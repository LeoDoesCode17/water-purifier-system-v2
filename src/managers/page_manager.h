#pragma once
#include <LiquidCrystal_I2C.h>
#include "ui/page_id.h"

namespace page_manager
{
    void initialize(LiquidCrystal_I2C &lcd);
    void update();
    void navigate_to(pages::PageId id);
} // namespace page_manager
