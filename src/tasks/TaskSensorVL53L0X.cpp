#include "TaskSensorVL53L0X.h"

VL53L0X sensor;

void TaskSensorVL53L0XLaser(void *pvParameters) {
    (void) pvParameters;

    xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    sensor.setTimeout(300);
    if (!sensor.init()) {
        Serial.println("[ERROR] VL53L0X init fail");
        xSemaphoreGive(xI2CMutex);
        vTaskDelete(NULL);
    }
    sensor.startContinuous(50);    // 20Hz
    xSemaphoreGive(xI2CMutex);

    while (1) {

        xSemaphoreTake(xI2CMutex, portMAX_DELAY);
        int distance = sensor.readRangeContinuousMillimeters();
        bool timeout = sensor.timeoutOccurred();
        xSemaphoreGive(xI2CMutex);

        if (timeout) {
            Serial.println("[WARN] VL53 Timeout!");
        } else {
            Serial.printf("[VL53] Distance: %d mm\n", distance);
        }

        if (distance < 20 || distance > 45) {
            tone(SPEAKER, 3000, 30);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        else {
            noTone(SPEAKER);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


void createTaskVL53L0X() {
    xTaskCreatePinnedToCore(TaskSensorVL53L0XLaser, "VL53L0X", 4096, NULL, 2, NULL, 0);
}
