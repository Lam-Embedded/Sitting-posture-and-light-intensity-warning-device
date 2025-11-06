#include <Arduino.h>
#include <WiFi.h>
#include "setUpWifi.h"

#include "tasks/TaskButton.h"
#include "tasks/TaskSensorVL53L0X.h"
#include "tasks/TaskSensorTemt6000.h"
#include "tasks/TaskAudio.h"
#include "tasks/TaskBuzzer.h"
#include "tasks/TaskSendData.h"

QueueHandle_t xQueueButton;

// Hàm khởi tạo các task (chạy sau khi WiFi kết nối thành công)
void initTasks() {
    xQueueButton = xQueueCreate(10, sizeof(uint8_t));
    if (xQueueButton == NULL) {
        Serial.println("❌ Không thể tạo queue Button!");
        while (1);
    }

    // Tạo các task
    createTaskButton();
    createTaskVL53L0X();
    createTaskTemt6000();
    createTaskAudio();
    createTaskBuzzer();
    createTaskSendData();
}

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println("\n=== ESP32 Smart Device Boot ===");

    // 1️⃣ Khởi tạo WiFi hoặc Captive Portal
    if (initWiFiConfig()) {
        Serial.println("✅ Wi-Fi Connected! IP: " + WiFi.localIP().toString());
        initTasks();  // 2️⃣ Chỉ tạo task sau khi WiFi sẵn sàng
    } else {
        Serial.println("🌐 Running in Captive Portal Mode...");
        // Captive portal sẽ được xử lý trong setUpWifi.cpp (loop)
    }
}

void loop() {
    // Xử lý captive portal nếu đang ở AP mode
    handleWiFiPortalLoop();

    // Nếu đang ở STA mode, có thể dùng delay nhẹ để giảm tải CPU
    if (WiFi.getMode() == WIFI_STA) {
        delay(500);
    } else {
        delay(10);
    }
}
