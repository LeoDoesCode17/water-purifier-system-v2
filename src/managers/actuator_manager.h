#pragma once
#include "core/type.h"

namespace actuator_manager
{
    void initialize();
    void turn_on(Actuator actuator);
    void turn_off(Actuator actuator);
}