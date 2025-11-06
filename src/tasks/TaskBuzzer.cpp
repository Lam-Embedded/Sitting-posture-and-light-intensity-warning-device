#include "TaskBuzzer.h"

extern QueueHandle_t xQueueButton;

void TaskBuzzer(void *pvParameters) {
    (void) pvParameters;
    pinMode(SPEAKER, OUTPUT);
    uint8_t receivedValue = 0;

    while (1) {
        if (xQueueReceive(xQueueButton, &receivedValue, portMAX_DELAY) == pdTRUE) {
            if (receivedValue == 1) {
                digitalWrite(27, HIGH);
                vTaskDelay(300 / portTICK_PERIOD_MS);
                digitalWrite(27, LOW);
            }
        }
    }
}

void createTaskBuzzer() {
    xTaskCreatePinnedToCore(TaskBuzzer, "Buzzer", 1536, NULL, 1, NULL, 1);
}
