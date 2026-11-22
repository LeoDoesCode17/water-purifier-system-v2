#include "uart1.h"

namespace
{
    static uart_sensor_t sensor_data;
    static constexpr int BAUD_RATE = 9600;
}

namespace uart1
{
    void initialize()
    {
        Serial1.begin(BAUD_RATE, SERIAL_8N1, pin::_RX1, pin::_TX1);
    }

    void update()
    {
        if (Serial1.available())
        {
            String line = Serial1.readStringUntil('\n');
            line.trim();
            if (line.isEmpty())
            {
                return;
            }

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, line);

            if (err)
            {
                Serial.printf("JSON error: %s | raw: %s\n", err.c_str(), line.c_str());
                return;
            }

            sensor_data.ph_voltage = doc["ph_voltage"] | -99.0f;
            sensor_data.tds_voltage = doc["tds_voltage"] | -99.0f;
            sensor_data.turbidity_voltage = doc["turbidity_voltage"] | -99.0f;
            sensor_data.distance_cm = doc["distance_cm"];
            sensor_data.temperature_celcius = doc["temperature_celcius"] | -99.0f;
        }
    }

    const uart_sensor_t& get_uart_sensor_data()
    {
        return sensor_data;
    }

}