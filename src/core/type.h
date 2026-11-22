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

// Wi-Fi credentials entered in the web form
struct WifiConfig
{
    String ssid;
    String password; // Empty for open networks
};

// MQTT broker settings entered in the web form
struct MqttConfig
{
    String host;
    uint16_t port = 1883; // Stored as a number (validated 1-65535); 1883 is the MQTT default
    String username;      // Optional: empty means "connect without authentication"
    String password;      // Optional
};
