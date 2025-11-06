#include "TaskManager.h"

extern QueueHandle_t xQueueButton;
extern QueueHandle_t xQueueAudio;
extern QueueHandle_t xQueueVl53L0;
extern QueueHandle_t xQueueTEMT6000;

void TaskManager(void *pvParameters) {
    (void) pvParameters;

    uint8_t readButtonState = 0;
    uint8_t readVL53L0XState = 0;
    uint8_t readTEMT6000State = 0;

    while (1) {
        if (xQueueReceive(xQueueButton, &readButtonState, portMAX_DELAY) == pdTRUE) {
            if (readButtonState == 0) {
                
            }
            if (readButtonState == 1) {
                
            }
        }

        if (xQueueReceive(xQueueVl53L0, &readVL53L0XState, portMAX_DELAY) == pdTRUE) {
            uint8_t msg = readVL53L0XState;
            xQueueSend(xQueueAudio, &msg, portMAX_DELAY);
        }

        if (xQueueReceive(xQueueTEMT6000, &readTEMT6000State, portMAX_DELAY) == pdTRUE) {
            uint8_t msg = readTEMT6000State;
            xQueueSend(xQueueAudio, &msg, portMAX_DELAY);
        }
    }
}

void createTaskManager() {
    xTaskCreatePinnedToCore(TaskManager, "Manager", 1536, NULL, 1, NULL, 1);
}
