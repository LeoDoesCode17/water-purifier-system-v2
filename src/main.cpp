#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "managers/actuator_manager.h"
#include "managers/sensor_manager.h"
#include "managers/network_manager.h"
#include "managers/page_manager.h"
#include "managers/queue_manager.h"
#include "core/type.h"

namespace
{
  // ---- Task settings ----
  static constexpr uint32_t TASK_STACK_BYTES = 8192; // Wi-Fi/MQTT/printf need more than 4096
  static constexpr uint32_t LIGHT_TASK_STACK_BYTES = 2048;
  static constexpr UBaseType_t TASK_PRIORITY = 1;
  static constexpr uint8_t SDA_PIN = 8;
  static constexpr uint8_t SCL_PIN = 9;
  // initialize object for display
  LiquidCrystal_I2C lcd(0x27, 20, 4);
}

void mainTask(void *pv)
{
  Serial.printf("[RTOS] Core-%d - main task started\n", xPortGetCoreID());
  for (;;)
  {
    // network_manager::update_on_core_1();
    network_manager::update_ap(); // config web server (only does work in AP mode)
    page_manager::update();
    vTaskDelay(pdMS_TO_TICKS(1)); // yield so the idle task / watchdog can run
  }
}

// Core 0: Wi-Fi + MQTT connect/reconnect (publish/receive later).
void networkTask(void *pv)
{
  Serial.printf("[RTOS] Core-%d - network task started\n", xPortGetCoreID());
  network_manager::initialize_on_core_0();
  for (;;)
  {
    network_manager::update_on_core_0(); 
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void uartSensorProduceTask(void *pv)
{
  Serial.printf("[RTOS] Core-%d - uart sensor produce task started\n", xPortGetCoreID());
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(100);
  for (;;)
  {
    const uart_sensor_t data = network_manager::get_uart_sensor_data();
    queue_manager::enqueue_sensor_data(data);
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

void uartSensorConsumeTask(void *pv)
{
  Serial.printf("[RTOS] Core-%d - uart sensor consume task started\n", xPortGetCoreID());
  uart_sensor_t sensor_data{};
  for (;;)
  {
    if (queue_manager::dequeue_sensor_data(sensor_data, portMAX_DELAY))
    {
      sensor_manager::set_uart_sensor_data(sensor_data);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  // Initialize custom I2C pins for ESP32-S3
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize the LCD module
  lcd.init();

  queue_manager::initialize();

  actuator_manager::initialize();
  sensor_manager::initialize();
  page_manager::initialize(lcd);
  network_manager::initialize_on_core_1();

  xTaskCreatePinnedToCore(networkTask, "NetworkTask", TASK_STACK_BYTES, NULL, TASK_PRIORITY, NULL, 0);
  xTaskCreatePinnedToCore(uartSensorProduceTask, "UartSensorProduceTask", LIGHT_TASK_STACK_BYTES, NULL, TASK_PRIORITY, NULL, 0);
  xTaskCreatePinnedToCore(mainTask, "MainTask", TASK_STACK_BYTES, NULL, TASK_PRIORITY, NULL, 1);
  xTaskCreatePinnedToCore(uartSensorConsumeTask, "UartSensorConsumeTask", LIGHT_TASK_STACK_BYTES, NULL, TASK_PRIORITY, NULL, 1);
}

void loop()
{
  vTaskDelete(NULL);
}
