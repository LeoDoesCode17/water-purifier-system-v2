#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "managers/actuator_manager.h"
#include "managers/sensor_manager.h"
#include "managers/network_manager.h"
#include "managers/page_manager.h"

namespace
{
  // ---- Task settings ----
  static constexpr uint32_t TASK_STACK_BYTES = 8192; // Wi-Fi/MQTT/printf need more than 4096
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
    network_manager::update_on_core_1();
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
    network_manager::update_on_core_0(); // was update_on_core_1(): LoRa was being driven from BOTH cores
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void setup()
{
  Serial.begin(115200);
  // Initialize custom I2C pins for ESP32-S3
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize the LCD module
  lcd.init();

  actuator_manager::initialize();
  sensor_manager::initialize();
  page_manager::initialize(lcd);
  network_manager::initialize_on_core_1();

  xTaskCreatePinnedToCore(networkTask, "NetworkTask", TASK_STACK_BYTES, NULL, TASK_PRIORITY, NULL, 0);
  xTaskCreatePinnedToCore(mainTask, "MainTask", TASK_STACK_BYTES, NULL, TASK_PRIORITY, NULL, 1);
}

void loop()
{
  vTaskDelete(NULL);
}
