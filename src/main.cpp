#include <Arduino.h>
#include <WiFi.h>

#include "setUpWifi.h"
#include "tasks/TaskButton.h"
#include "tasks/TaskSensorVL53L0X.h"
#include "tasks/TaskSensorTemt6000.h"
#include "tasks/TaskAudio.h"
#include "tasks/TaskBuzzer.h"

QueueHandle_t xQueueButton;

void setup() {
    Serial.begin(115200);
    // PinMode
    pinMode(BUTTON_START, INPUT_PULLUP);
    // Tạo queue
    xQueueButton = xQueueCreate(10, sizeof(uint8_t));

    if (xQueueButton == NULL) {
        Serial.println("Không thể tạo queue!");
        while (1);
    }
    
    // Tao cac task
    createTaskButton();
    createTaskVL53L0X();
    createTaskTemt6000();
    createTaskAudio();
    createTaskBuzzer();
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}