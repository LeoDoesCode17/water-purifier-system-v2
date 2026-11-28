#include "actuator_manager.h"
#include "core/pin.h"
#include <ESP32Servo.h>

namespace
{
    static Servo servo;
    const uint8_t CLOSED_SERVO_DEGREE = 0;
    const uint8_t OPENED_SERVO_DEGREE = 90;
    static bool _is_valve_open = false;
    static bool _is_pump1_on = false;
    static bool _is_pump2_on = false;
    static bool _is_motor_gear_on = false;
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
            digitalWrite(pin::MOTOR_GEAR, HIGH);
            _is_motor_gear_on = true;
            break;
        case Actuator::PUMP_1:
            digitalWrite(pin::PUMP_1, HIGH);
            _is_pump1_on = true;
            break;
        case Actuator::PUMP_2:
            digitalWrite(pin::PUMP_2, HIGH);
            _is_pump2_on = true;
            break;
        case Actuator::VALVE:
            servo.write(OPENED_SERVO_DEGREE);
            _is_valve_open = true;
            break;
        }
    }
    void turn_off(Actuator actuator)
    {
        switch (actuator)
        {
        case Actuator::MOTOR_GEAR:
            digitalWrite(pin::MOTOR_GEAR, LOW);
            _is_motor_gear_on = false;
            break;
        case Actuator::PUMP_1:
            digitalWrite(pin::PUMP_1, LOW);
            _is_pump1_on = false;
            break;
        case Actuator::PUMP_2:
            digitalWrite(pin::PUMP_2, LOW);
            _is_pump2_on = false;
            break;
        case Actuator::VALVE:
            servo.write(CLOSED_SERVO_DEGREE);
            _is_valve_open = false;
            break;
        }
    }

    bool is_actuator_on(Actuator actuator)
    {
        switch (actuator)
        {
        case Actuator::MOTOR_GEAR:
            return _is_motor_gear_on;
        case Actuator::PUMP_1:
            return _is_pump1_on;
        case Actuator::PUMP_2:
            return _is_pump2_on;
        case Actuator::VALVE:
            return _is_valve_open;
        default:
            return false;
        }
    }
}