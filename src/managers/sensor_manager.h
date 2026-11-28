#pragma once
#include "core/type.h"

namespace sensor_manager
{
    void initialize();
    long getEncoderPosition();
    void setEncoderPosition(long position);
    bool wasEncoderButtonPressed();
    void set_uart_sensor_data(const uart_sensor_t &uart_sensor_data);
    const uart_sensor_t get_uart_sensor_data();
}