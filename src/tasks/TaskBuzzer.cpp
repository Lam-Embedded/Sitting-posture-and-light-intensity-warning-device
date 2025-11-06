#include "TaskBuzzer.h"

extern QueueHandle_t xQueueButton;
extern QueueHandle_t xQueueAudio;

void TaskBuzzer(void *pvParameters) {
    (void) pvParameters;
    pinMode(SPEAKER, OUTPUT);
    uint8_t receivedValueButton = 0;
    uint8_t receivedValueSensor = 0;

    while (1) {
        if (xQueueReceive(xQueueButton, &receivedValueButton, portMAX_DELAY) == pdTRUE) {
            if (receivedValueButton == 1 || receivedValueButton == 2 || receivedValueButton == 3 || receivedValueButton == 0) {
                digitalWrite(27, HIGH);
                vTaskDelay(300 / portTICK_PERIOD_MS);
                digitalWrite(27, LOW);
            }
            
        }

        if (xQueueReceive(xQueueAudio, &receivedValueSensor, portMAX_DELAY) == pdTRUE) {
            if (receivedValueSensor == 1 || receivedValueSensor == 2 || receivedValueSensor == 3 || receivedValueSensor == 4) {
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
