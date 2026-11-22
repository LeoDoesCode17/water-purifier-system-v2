#include "actuator_manager.h"
#include "core/pin.h"
#include <ESP32Servo.h>

namespace
{
    static Servo servo;
    const uint8_t CLOSED_SERVO_DEGREE = 0;
    const uint8_t OPENED_SERVO_DEGREE = 90;
} // namespace

namespace actuator_manager
{
    void initialize()
    {
        pinMode(pin::MOTOR_GEAR, OUTPUT);
        pinMode(pin::PUMP_1, OUTPUT);
        pinMode(pin::PUMP_2, OUTPUT);

        // init servo
        ESP32PWM::allocateTimer(0);
        ESP32PWM::allocateTimer(1);
        ESP32PWM::allocateTimer(2);
        ESP32PWM::allocateTimer(3);

        servo.setPeriodHertz(50); // Standard 50Hz frequency for analog servos

        // Attach the servo with standard microsecond pulse widths for 0 to 180 degrees
        // (Commonly 500us to 2400us/2500us depending on the exact SG90/MG996R variant)
        servo.attach(pin::VALVE, 500, 2400);
        servo.write(CLOSED_SERVO_DEGREE);
    }
    void turn_on(Actuator actuator)
    {
        switch (actuator)
        {
        case Actuator::MOTOR_GEAR:
            digitalWrite(pin::MOTOR_GEAR, LOW);
            break;
        case Actuator::PUMP_1:
            digitalWrite(pin::PUMP_1, HIGH);
            break;
        case Actuator::PUMP_2:
            digitalWrite(pin::PUMP_2, HIGH);
            break;
        case Actuator::VALVE:
            servo.write(OPENED_SERVO_DEGREE);
            break;
        }
    }
    void turn_off(Actuator actuator)
    {
        switch (actuator)
        {
        case Actuator::MOTOR_GEAR:
            digitalWrite(pin::MOTOR_GEAR, HIGH);
            break;
        case Actuator::PUMP_1:
            digitalWrite(pin::PUMP_1, LOW);
            break;
        case Actuator::PUMP_2:
            digitalWrite(pin::PUMP_2, LOW);
            break;
        case Actuator::VALVE:
            servo.write(CLOSED_SERVO_DEGREE);
            break;
        }
    }
}