#include "TaskSensorVL53L0X.h"

extern QueueHandle_t xQueueVL53L0X;
extern QueueHandle_t xQueueButton;

VL53L0X sensor;

void TaskSensorVL53L0XLaser(void *pvParameters) {
    (void) pvParameters;

    Wire.begin();
    Wire.setClock(400000);
    Serial.println("[INFO] Đang khởi tạo cảm biến VL53L0X...");

    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("[ERROR] Không tìm thấy cảm biến VL53L0X!");
        vTaskDelete(NULL);
    }

    Serial.println("[INFO] Cảm biến VL53L0X đã sẵn sàng!");

    int distance = 0;
    bool systemActive = false;   // trạng thái hệ thống (đang bật/tắt)
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
            distance = sensor.readRangeSingleMillimeters();

            if (sensor.timeoutOccurred()) {
                Serial.println("[WARN] Timeout khi đọc VL53L0X!");
            } else {
                uint8_t msgErrorDistance = 0;
                if (distance <= 20 || distance >= 45) {
                    msgErrorDistance = 1;
                }
                else {
                    msgErrorDistance = 0;
                }
                Serial.printf("Khoảng cách: %d mm\n", distance);
                xQueueSend(xQueueVL53L0X, &msgErrorDistance, 0);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void createTaskVL53L0X() {
    xTaskCreatePinnedToCore(TaskSensorVL53L0XLaser, "VL53L0X", 4096, NULL, 2, NULL, 0);
}
