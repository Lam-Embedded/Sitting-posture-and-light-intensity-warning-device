#include "TaskSensorTemt6000.h"

extern QueueHandle_t xQueueTEMT6000;
extern QueueHandle_t xQueueButton;

const float ADC_REF = 3.3;   // Điện áp tham chiếu ADC của ESP32
const int ADC_MAX = 4095;

void TaskSensorTemt6000(void *pvParameters) {
    (void) pvParameters;

    pinMode(TEMT6000_PIN, INPUT);
    analogReadResolution(12);

    bool systemActive = false;
    uint8_t buttonStatus = 0;

    while (1) {
        if (xQueueReceive(xQueueButton, &buttonStatus, 0) == pdTRUE) {
            if (buttonStatus == 1) {
                systemActive = !systemActive; // đảo trạng thái
                Serial.printf("[BUTTON] Hệ thống %s\n", systemActive ? "BẬT" : "TẮT");
            }
        }

        // chỉ đọc cảm biến khi hệ thống đang bật
        if (systemActive) {
            int sensorValue = analogRead(TEMT6000_PIN);
            float voltage = (sensorValue * ADC_REF) / ADC_MAX;
            float lux = voltage * 500.0;
            uint8_t msgErrorLux = 0;
            Serial.printf("ADC:%4d | Volt:%.2fV | Lux:%.1f\n", sensorValue, voltage, lux);

            msgErrorLux = (lux <= 400 || lux >= 750) ? 1 : 0;
            xQueueSend(xQueueTEMT6000, &msgErrorLux, 0);
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

void createTaskTemt6000() {  
    xTaskCreatePinnedToCore(TaskSensorTemt6000, "Temt6000", 4096, NULL, 2, NULL, 1);
}
