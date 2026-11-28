#pragma once
#include "core/type.h"

namespace queue_manager
{
    void initialize();
    bool enqueue_sensor_data(const uart_sensor_t &sensor_data);
    bool dequeue_sensor_data(uart_sensor_t &sensor_data, TickType_t timeout);
}