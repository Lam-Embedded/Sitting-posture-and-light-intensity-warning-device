#include <Arduino.h>
#include <WiFi.h>
#include "setUpWifi.h"

#include "tasks/TaskSensorVL53L0X.h"

void initTasks() {
    // Tạo các task
    createTaskVL53L0X();
}

void setup() {
    Serial.begin(115200);
    initTasks();
}

void loop() {

}
