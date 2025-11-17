#include "TaskBuzzer.h"

extern QueueHandle_t xQueueVL53L0X;
extern QueueHandle_t xQueueTEMT6000;

void TaskBuzzer(void *pvParameters) {
    (void) pvParameters;
    pinMode(SPEAKER, OUTPUT);

    uint8_t receivedValueButton = 0;
    uint8_t readDistanceData = 0;
    uint8_t readLuxData = 0;

    bool distanceError = false;
    bool luxError = false;

    while (1) {
        // --- 2. Đọc dữ liệu cảm biến ---
        if (xQueuePeek(xQueueVL53L0X, &readDistanceData, 0) == pdTRUE) {
            distanceError = (readDistanceData == 1);
        }

        if (xQueuePeek(xQueueTEMT6000, &readLuxData, 0) == pdTRUE) {
            luxError = (readLuxData == 1);
        }

        // --- 3. Nếu có lỗi từ cảm biến -> bip liên tục ---
        if (distanceError || luxError) {
            digitalWrite(SPEAKER, HIGH);
            vTaskDelay(pdMS_TO_TICKS(150));
            digitalWrite(SPEAKER, LOW);
            vTaskDelay(pdMS_TO_TICKS(150));
        } else {
            vTaskDelay(pdMS_TO_TICKS(200)); // nghỉ, tiết kiệm CPU
        }
    }
}

void createTaskBuzzer() {
    xTaskCreatePinnedToCore(TaskBuzzer, "Buzzer", 4096, NULL, 1, NULL, 1);
}
