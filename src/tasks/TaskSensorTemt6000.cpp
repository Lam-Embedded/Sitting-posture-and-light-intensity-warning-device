#include "TaskSensorTemt6000.h"

// extern QueueHandle_t xQueueTEMT6000;

const float ADC_REF = 3.3;   // Điện áp tham chiếu ADC của ESP32
const int ADC_MAX = 4095;

void TaskSensorTemt6000(void *pvParameters) {
    (void) pvParameters;

    pinMode(TEMT6000_PIN, INPUT);
    analogReadResolution(12);

    float lastLux = 0;

    while (1) {
        int sensorValue = analogRead(TEMT6000_PIN);
        float voltage = (sensorValue * ADC_REF) / ADC_MAX;
        float lux = voltage * 500.0;

        // Serial.printf("Light: %d lux\n", sensorValue);
        Serial.printf("ADC:%4d | Volt:%.2fV | Lux:%.1f\n", sensorValue, voltage, lux);
        // if (fabs(lux - lastLux) > 5.0) {
        //     Serial.printf("ADC:%4d | Volt:%.2fV | Lux:%.1f\n", sensorValue, voltage, lux);
        //     // if (lux <= 400 || lux >= 750) {
        //     //     uint8_t msg = 3;
        //     //     xQueueSend(xQueueTEMT6000, &msg, portMAX_DELAY);
        //     // }
        //     // else {
        //     //     uint8_t msg = 2;
        //     //     xQueueSend(xQueueTEMT6000, &msg, portMAX_DELAY);
        //     // }

        //     lastLux = lux;
        // }

        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

void createTaskTemt6000() {  
    xTaskCreatePinnedToCore(TaskSensorTemt6000, "Temt6000", 4096, NULL, 2, NULL, 1);
}
