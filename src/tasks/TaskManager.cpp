#include "TaskManager.h"

extern QueueHandle_t xQueueButton;
// extern QueueHandle_t xQueueAudio;
extern QueueHandle_t xQueueVL53L0X;
// extern QueueHandle_t xQueueTEMT6000;
extern QueueHandle_t xQueueSendData;

void TaskManager(void *pvParameters) {
    (void) pvParameters;

    uint8_t readButtonState = 0;
    uint8_t readVL53L0XState = 0;
    // uint8_t readTEMT6000State = 0;

    while (1) {
        // if (xQueueReceive(xQueueButton, &readButtonState, portMAX_DELAY) == pdTRUE) {
        //     if (readButtonState == 0) {
                
        //     }
        //     if (readButtonState == 1) {
                
        //     }
        // }

        if (xQueueReceive(xQueueVL53L0X, &readVL53L0XState, portMAX_DELAY) == pdTRUE) {
            if (readVL53L0XState == 1) {
                // gui du lieu den WhatApp
                uint8_t msgData = 1;
                xQueueSend(xQueueSendData, &msgData, portMAX_DELAY);
                // gui tin hieu bao dong cho buzzer, audio
                // uint8_t msg = 1;
                // xQueueSend(xQueueAudio, &msg, portMAX_DELAY);
            }
        }

        // if (xQueueReceive(xQueueTEMT6000, &readTEMT6000State, portMAX_DELAY) == pdTRUE) {
        //     if (readTEMT6000State == 3) {
        //         // Gui du lieu den WhatApp
        //         uint8_t msgData = 2;
        //         xQueueSend(xQueueSendData, &msgData, portMAX_DELAY);
        //         // gui tin hieu bao dong cho buzzer, audio
        //         uint8_t msg = 2;
        //         xQueueSend(xQueueAudio, &msg, portMAX_DELAY);
        //     }
        // }
    }
}

void createTaskManager() {
    xTaskCreatePinnedToCore(TaskManager, "Manager", 1536, NULL, 1, NULL, 1);
}
