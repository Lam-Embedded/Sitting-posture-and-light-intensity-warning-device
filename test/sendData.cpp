#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <UrlEncode.h>

const char* ssid = "Vinatedia";
const char* password = "techmedia";
String apiKey   = "a02a5f66-8bed-477f-b284";
String phone    = "84896523354";  // Ví dụ: "84901234567"

// Khai báo trước
void sendMessage(String message);

void setup() {
  Serial.begin(115200);

  sendMessage("Lam - Nga, he hehehehe");
}

void loop() {}

void sendMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.whatabot.io/whatsapp/sendMessage?apikey=" + apiKey +
                 "&phone=" + phone +
                 "&text=" + urlEncode(message);

    Serial.println("Requesting: " + url);
    http.begin(url);

    int httpResponseCode = http.GET();

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.println("Error sending GET request");
    }

    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}


