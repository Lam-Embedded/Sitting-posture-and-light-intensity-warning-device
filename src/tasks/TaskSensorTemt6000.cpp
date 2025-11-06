#include "TaskSensorTemt6000.h"

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

        if (fabs(lux - lastLux) > 5.0) {
            Serial.printf("Light: %.1f lux\n", lux);
            lastLux = lux;
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // đọc mỗi 200ms
    }
}

void createTaskTemt6000() {
    xTaskCreatePinnedToCore(TaskSensorTemt6000, "Temt6000", 1536, NULL, 2, NULL, 1);
}
