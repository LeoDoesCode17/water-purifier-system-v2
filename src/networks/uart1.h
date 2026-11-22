#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "core/type.h"
#include "core/pin.h"

namespace uart1
{
    void initialize();
    void update();
    const uart_sensor_t& get_uart_sensor_data();
}