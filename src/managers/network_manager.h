#pragma once
#include "core/type.h"
#include <ArduinoJson.h>

namespace network_manager
{
    void initialize();
    void connect();
    const uart_sensor_t& get_uart_sensor();
} // namespace network_manager
