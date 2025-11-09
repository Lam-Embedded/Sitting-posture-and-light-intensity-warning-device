#include "TaskBuzzer.h"

extern QueueHandle_t xQueueButton;
// extern QueueHandle_t xQueueAudio;

void TaskBuzzer(void *pvParameters) {
    (void) pvParameters;
    pinMode(SPEAKER, OUTPUT);

    uint8_t receivedValueButton = 0;
    uint8_t receivedValueSensor = 0;

    while (1) {
        if (xQueueReceive(xQueueButton, &receivedValueButton, pdMS_TO_TICKS(100)) == pdTRUE) {
            Serial.printf("[Buzzer] Nhận nút: %d\n", receivedValueButton);
            digitalWrite(SPEAKER, HIGH);
            vTaskDelay(300 / portTICK_PERIOD_MS);
            digitalWrite(SPEAKER, LOW);
            vTaskDelay(300 / portTICK_PERIOD_MS);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);

        // if (xQueueReceive(xQueueAudio, &receivedValueSensor, portMAX_DELAY) == pdTRUE) {
        //     if (receivedValueSensor == 1 || receivedValueSensor == 2 || receivedValueSensor == 3 || receivedValueSensor == 4) {
        //         digitalWrite(27, HIGH);
        //         vTaskDelay(300 / portTICK_PERIOD_MS);
        //         digitalWrite(27, LOW);
        //         vTaskDelay(300 / portTICK_PERIOD_MS);
        //     }
        // }
    }
}

void createTaskBuzzer() {
    xTaskCreatePinnedToCore(TaskBuzzer, "Buzzer", 4096, NULL, 1, NULL, 1);
}
