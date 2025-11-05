#include <WiFiManager.h>
#include "setUpWifi.h"

void wifiSetup() {
    WiFi.mode(WIFI_STA);
    WiFiManager wm;
    if (!wm.autoConnect("SMART_EYES")) {
        Serial.println("Failed to connect");
        ESP.restart();
    } else {
        Serial.println("WiFi connected!");
    }

    Serial.println(WiFi.localIP());
}