#pragma once
#include <Arduino.h>

enum class Actuator : uint8_t
{
    MOTOR_GEAR,
    PUMP_1,
    PUMP_2,
    VALVE,
    NONE
};

typedef struct
{
    float ph_voltage;
    float tds_voltage;
    float turbidity_voltage;
    float distance_cm;
    float temperature_celcius;
} uart_sensor_t;
