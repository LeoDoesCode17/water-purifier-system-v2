#include "uart1.h"

namespace
{
    static uart_sensor_t sensor_data;
    static constexpr int BAUD_RATE = 9600;
    static constexpr size_t RX_BUF_SIZE = 256;
    static char rx_buf[RX_BUF_SIZE];
    static size_t rx_idx = 0;

    void parse_line(const char *line)
    {
        if (line[0] == '\0')
            return;

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, line);
        if (err)
        {
            Serial.printf("JSON error: %s | raw: %s\n", err.c_str(), line);
            return;
        }

        sensor_data.ph_voltage = doc["ph_voltage"] | -99.0f;
        sensor_data.tds_voltage = doc["tds_voltage"] | -99.0f;
        sensor_data.turbidity_voltage = doc["turbidity_voltage"] | -99.0f;
        sensor_data.distance_cm = doc["distance_cm"] | -99.0f;
        sensor_data.temperature_celcius = doc["temperature_celcius"] | -99.0f;
    }
}

namespace uart1
{
    void initialize()
    {
        Serial1.begin(BAUD_RATE, SERIAL_8N1, pin::_RX1, pin::_TX1);
    }

    void update()
    {
        while (Serial1.available() > 0)
        {
            char c = static_cast<char>(Serial1.read());
            if (c == '\r')
                continue;

            if (c == '\n')
            {
                rx_buf[rx_idx] = '\0';
                parse_line(rx_buf);
                rx_idx = 0;
            }
            else if (rx_idx < RX_BUF_SIZE - 1)
            {
                rx_buf[rx_idx++] = c;
            }
            else
            {
                // Line overflow: reset buffer to recover cleanly
                rx_idx = 0;
            }
        }
    }

    const uart_sensor_t &get_sensor_data()
    {
        return sensor_data;
    }
}
