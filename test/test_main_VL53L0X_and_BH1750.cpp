#include <Arduino.h>
#include <WiFi.h>
#include "setUpWifi.h"

#include "I2CBus.h"
#include "tasks/TaskSensorVL53L0X.h"
#include "tasks/TaskSensorBH1750.h"

void initTasks() {
    createTaskVL53L0X();
    createTaskBH1750();
}

void setup() {
    Serial.begin(115200);
    delay(200);

    // Khởi tạo I2C bus dùng chung cho toàn bộ hệ thống
    initI2CBus();

    // Khởi tạo task
    initTasks();
}

void loop() {
    // Không dùng loop
}
