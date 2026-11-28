#include "sensor_manager.h"
#include "sensors/rotary_encoder.h"
#include "core/pin.h"

namespace
{
    static uart_sensor_t _uart_sensor_data{};
}

namespace sensor_manager
{
    void initialize()
    {
        rotary_encoder::initialize(pin::ROTARY_ENCODER_DT, pin::ROTARY_ENCODER_CLK, pin::ROTARY_ENCODER_SW);
    }

    long getEncoderPosition()
    {
        return rotary_encoder::getPosition();
    }

    void setEncoderPosition(long position)
    {
        rotary_encoder::setPosition(position);
    }

    bool wasEncoderButtonPressed()
    {
        return rotary_encoder::wasButtonPressed();
    }

    void set_uart_sensor_data(const uart_sensor_t &uart_sensor_data)
    {
        _uart_sensor_data = {
            .ph_voltage = uart_sensor_data.ph_voltage,
            .tds_voltage = uart_sensor_data.tds_voltage,
            .turbidity_voltage = uart_sensor_data.turbidity_voltage,
            .distance_cm = uart_sensor_data.distance_cm,
            .temperature_celcius = uart_sensor_data.temperature_celcius,
        };
    }

    const uart_sensor_t get_uart_sensor_data()
    {
        return _uart_sensor_data;
    }

}