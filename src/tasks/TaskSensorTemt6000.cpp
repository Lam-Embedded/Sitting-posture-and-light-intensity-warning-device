#include "TaskSensorTemt6000.h"

const float ADC_REF = 3.3;   // Điện áp tham chiếu ADC của ESP32
const int ADC_MAX = 4095;

void TaskSensorTemt6000(void *pvParameters) {
    (void) pvParameters;

    while (1) {
        int sensorValue = analogRead(TEMT6000_PIN); 
        float voltage = (sensorValue * ADC_REF) / ADC_MAX;

        float lux = voltage * 500.0;

        Serial.print("Light: ");
        Serial.print(lux, 1);
        Serial.println(" lux");
    }
}

void createTaskTemt6000() {
    xTaskCreatePinnedToCore(TaskSensorTemt6000, "Temt6000", 2048, NULL, 2, NULL, 1);
}
