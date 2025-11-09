#include "TaskSensorVL53L0X.h"

extern QueueHandle_t xQueueVL53L0X;

VL53L0X sensor;

void TaskSensorVL53L0XLaser(void *pvParameters) {
    (void) pvParameters;

    Wire.begin();
    Wire.setClock(400000); // tăng tốc độ I2C
    Serial.println("[INFO] Đang khởi tạo cảm biến VL53L0X...");

    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("[ERROR] Không tìm thấy cảm biến VL53L0X!");
        vTaskDelete(NULL);
    }

    Serial.println("[INFO] Cảm biến VL53L0X đã sẵn sàng!");

    int distance = 0;
    int lastDistance = 0;

    while (1) {
        distance = sensor.readRangeSingleMillimeters();

        if (sensor.timeoutOccurred()) {
            Serial.println("[WARN] Timeout khi đọc VL53L0X!");
        } else if (abs(distance - lastDistance) > 10) {
            Serial.printf("Khoảng cách: %d mm\n", distance);
            lastDistance = distance;
        }

        if (distance <= 250 || distance >= 450) {
            uint8_t msg = 1;
            xQueueSend(xQueueVL53L0X, &msg, portMAX_DELAY);
        }
        else {
            uint8_t msg = 0;
            xQueueSend(xQueueVL53L0X, &msg, portMAX_DELAY);
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // tránh chiếm CPU
    }
}

void createTaskVL53L0X() {
    xTaskCreatePinnedToCore(TaskSensorVL53L0XLaser, "VL53L0X", 4096, NULL, 2, NULL, 0);
}