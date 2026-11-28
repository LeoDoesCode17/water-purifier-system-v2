#include "queue_manager.h"

namespace
{
    constexpr UBaseType_t UART_SENSOR_QUEUE_LENGTH = 10;
    QueueHandle_t uart_sensor_queue = nullptr;
} // namespace

namespace queue_manager
{
    void initialize()
    {
        uart_sensor_queue = xQueueCreate(UART_SENSOR_QUEUE_LENGTH, sizeof(uart_sensor_t));
        if (!uart_sensor_queue)
        {
            Serial.println("[FREE-RTOS] Creation queue failed! Halt system");
            while (true)
                delay(1000);
        }
    }

    bool enqueue_sensor_data(const uart_sensor_t &sensor_data)
    {
        if (xQueueSend(uart_sensor_queue, &sensor_data, 0) == pdTRUE)
        {
            return true;
        }

        // full drop the oldest item then produce once
        uart_sensor_t discarded;
        xQueueReceive(uart_sensor_queue, &discarded, 0);
        if (xQueueSend(uart_sensor_queue, &sensor_data, 0) == pdTRUE)
        {
            return true;
        }

        return false;
    }

    bool dequeue_sensor_data(uart_sensor_t &sensor_data, TickType_t timeout)
    {
        return xQueueReceive(uart_sensor_queue, &sensor_data, timeout) == pdTRUE;
    }

}