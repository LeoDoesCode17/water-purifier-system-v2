#include <Arduino.h>
#include "managers/actuator_manager.h"
#include "managers/sensor_manager.h"
#include "managers/network_manager.h"

void setup() {
  Serial.begin(115200);
  actuator_manager::initialize();
  sensor_manager::initialize();
  network_manager::initialize();
}

void loop() {
  // put your main code here, to run repeatedly:
}
