#ifndef SETUPWIFI_H
#define SETUPWIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

// Hàm khởi tạo WiFi hoặc Captive Portal
bool initWiFiConfig();

// Hàm vòng lặp xử lý captive portal
void handleWiFiPortalLoop();

String getSavedApiKey();
String getSavedPhone();

#endif
