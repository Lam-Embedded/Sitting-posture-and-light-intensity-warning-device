#include "TaskSendData.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include "setUpWifi.h"

extern QueueHandle_t xQueueVL53L0X;
extern QueueHandle_t xQueueTEMT6000;

void sendTestMessage(const String &msg);

uint8_t readDistanceData = 0;
uint8_t readLuxData = 0;

void TaskSendData(void *pvParameters) {
    (void) pvParameters;

    // pinMode(BUTTON_SPEAKER, INPUT_PULLUP);

    uint8_t lastDistanceError = 0;
    uint8_t lastLuxError = 0;

    while(1) {
        if(digitalRead(BUTTON_SPEAKER) == LOW) {
            sendTestMessage("Hello!!!");
            
        }
        vTaskDelay(pdMS_TO_TICKS(200)); // tránh chiếm CPU
    }

    while (1) {
        // Đọc VL53L0X (non-blocking)
        if (xQueuePeek(xQueueVL53L0X, &readDistanceData, 0) == pdTRUE) {
            if (readDistanceData != lastDistanceError) {
                lastDistanceError = readDistanceData;
                if (readDistanceData == 1 && WiFi.status() == WL_CONNECTED) {
                    sendTestMessage("⚠️ Tư thế ngồi sai! Hãy điều chỉnh lại.");
                }
            }
        }

        // Đọc TEMT6000 (non-blocking)
        if (xQueuePeek(xQueueTEMT6000, &readLuxData, 0) == pdTRUE) {
            if (readLuxData != lastLuxError) {
                lastLuxError = readLuxData;
                if (readLuxData == 1 && WiFi.status() == WL_CONNECTED) {
                    sendTestMessage("💡 Cường độ ánh sáng không phù hợp, hãy kiểm tra lại!");
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // tránh chiếm CPU
    }
}

void createTaskSendData() {
    xTaskCreatePinnedToCore(TaskSendData, "TaskSendData", 8192, NULL, 1, NULL, 1);
}

void sendTestMessage(const String &msg) {
    String apiKey = getSavedApiKey();
    String phone  = getSavedPhone();

    if (apiKey.isEmpty() || phone.isEmpty()) {
        Serial.println("❌ Thiếu API key hoặc số điện thoại!");
        return;
    }

    HTTPClient http;
    http.setTimeout(3000); // timeout 3s

    String url = "https://api.whatabot.io/whatsapp/sendMessage?apikey=" + apiKey +
                 "&phone=" + phone +
                 "&text=" + urlEncode(msg);

    http.begin(url);
    int code = http.GET();

    if (code > 0)
        Serial.printf("[SEND] %s\n", http.getString().c_str());
    else
        Serial.printf("[ERROR] HTTP code: %d\n", code);

    http.end();
}
