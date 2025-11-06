#include "TaskSendData.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>
#include "setUpWifi.h"

void sendTestMessage(const String &msg);

void TaskSendData(void *pvParameters) {
    (void) pvParameters;

    if (WiFi.status() == WL_CONNECTED) {
        sendTestMessage("Hello Nga ngo!!!");
    } 

    while (1) {
        // if (WiFi.status() == WL_CONNECTED) {
        //     sendTestMessage("Hello Nga ngo!!!");
        // } else {
        //     Serial.println("⚠️ Wi-Fi not connected, skipping send...");
        // }
        // vTaskDelay(pdMS_TO_TICKS(10000));  // gửi mỗi 10 giây
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

    Serial.println("🌐 Sending: " + url);

    http.begin(url);
    int code = http.GET();
    Serial.printf("HTTP Response: %d\n", code);
    if (code > 0)
        Serial.println(http.getString());
    http.end();
}
