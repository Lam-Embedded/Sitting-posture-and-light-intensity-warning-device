#include "TaskSendData.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include "setUpWifi.h"

extern QueueHandle_t xQueueSendData;

void sendTestMessage(const String &msg);

uint8_t readDataState = 0;

void TaskSendData(void *pvParameters) {
    (void) pvParameters;

    while (1) {
        if (xQueueReceive(xQueueSendData, &readDataState, portMAX_DELAY) == pdTRUE) {
            if (readDataState == 1) {
                if (WiFi.status() == WL_CONNECTED) {
                    sendTestMessage("Đã ngồi sai tư thế!!!");
                }
            }
            if (readDataState == 2) {
                if (WiFi.status() == WL_CONNECTED) {
                    sendTestMessage("Ánh sáng đang có vấn đề!!!!");
                }
            }
        }
    }
}

void createTaskSendData() {
    xTaskCreatePinnedToCore(TaskSendData, "TaskSendData", 8192, NULL, 1, NULL, 1);
}

void sendTestMessage(const String &msg) {
    String apiKey = getSavedApiKey();
    String phone  = getSavedPhone();

    if (apiKey.isEmpty() || phone.isEmpty()) {
        Serial.println("❌ Missing API key or phone number!");
        return;
    }

    HTTPClient http;
    String url = "https://api.whatabot.io/whatsapp/sendMessage?apikey=" + apiKey +
                 "&phone=" + phone +
                 "&text=" + urlEncode(msg);

    // Serial.println("🌐 Sending: " + url);

    http.begin(url);
    int code = http.GET();
    // Serial.printf("HTTP Response: %d\n", code);
    if (code > 0)
        Serial.println(http.getString());
    http.end();
}
