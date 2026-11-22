#include "network_manager.h"
#include "networks/uart1.h"

namespace network_manager
{
    void initialize()
    {
        uart1::initialize();
    }

    void connect()
    {
    }

    const uart_sensor_t &get_uart_sensor()
    {
        uart1::update();
        return uart1::get_uart_sensor_data();
    }

}