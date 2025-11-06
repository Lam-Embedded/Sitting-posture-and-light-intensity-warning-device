#include "setUpWifi.h"

#define AP_SSID "Device-Setup"
const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress NET_MASK(255, 255, 255, 0);
const byte DNS_PORT = 53;

WebServer server(80);
DNSServer dnsServer;
Preferences prefs;

// Keys trong Preferences
const char* PREF_NAMESPACE = "config";
const char* KEY_SSID   = "wifi_ssid";
const char* KEY_PASS   = "wifi_pass";
const char* KEY_APIKEY = "apikey";
const char* KEY_PHONE  = "phone";

String savedSSID, savedPass, savedApiKey, savedPhone;

static bool inCaptivePortal = false;

/*============================
  RESET CONFIG
=============================*/
void resetConfig() {
    Serial.println("🔄 Resetting saved Wi-Fi and API configuration...");
    prefs.begin(PREF_NAMESPACE, false);
    prefs.clear();
    prefs.end();
    delay(1000);
    ESP.restart();
}

/*============================
  CONNECT TO SAVED WIFI
=============================*/
bool connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());
    Serial.print("Connecting to saved Wi-Fi");

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    return (WiFi.status() == WL_CONNECTED);
}

/*============================
  CAPTIVE PORTAL SETUP
=============================*/
void startCaptivePortal() {
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_IP, NET_MASK);
    WiFi.softAP(AP_SSID);
    delay(200);

    dnsServer.start(DNS_PORT, "*", AP_IP);

    // Route cơ bản
    server.on("/", HTTP_GET, []() {
        String page = "<!doctype html><html><head><meta charset='utf-8'>";
        page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
        page += "<title>Device Setup</title>";
        page += "<style>body{font-family:Arial;padding:20px;}label{display:block;margin-top:10px;}input{width:100%;padding:8px;}button{margin-top:15px;padding:10px;width:100%;}</style>";
        page += "</head><body><h2>ESP32 Configuration</h2>";
        page += "<form method='POST' action='/save'>";
        page += "<label>Wi-Fi SSID</label><input name='ssid' required>";
        page += "<label>Wi-Fi Password</label><input name='pass' type='password'>";
        page += "<label>API Key</label><input name='apikey' required>";
        page += "<label>Phone Number</label><input name='phone' required>";
        page += "<button type='submit'>Save & Connect</button></form>";
        page += "<p style='color:#666;font-size:0.9em'>Hold BOOT button to reset configuration.</p>";
        page += "</body></html>";
        server.send(200, "text/html", page);
    });

    server.on("/save", HTTP_POST, []() {
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        String apikey = server.arg("apikey");
        String phone = server.arg("phone");

        prefs.putString(KEY_SSID, ssid);
        prefs.putString(KEY_PASS, pass);
        prefs.putString(KEY_APIKEY, apikey);
        prefs.putString(KEY_PHONE, phone);

        server.send(200, "text/html", "<h3>Saving... Restarting...</h3>");
        delay(500);
        ESP.restart();
    });

    server.onNotFound([]() {
        server.sendHeader("Location", String("http://") + AP_IP.toString(), true);
        server.send(302, "text/plain", "");
    });

    server.begin();
    inCaptivePortal = true;

    Serial.printf("🌐 Captive portal started on AP '%s'\n", AP_SSID);
}

/*============================
  INIT WIFI LOGIC
=============================*/
bool initWiFiConfig() {
    pinMode(RESET_PIN, INPUT_PULLUP);
    if (digitalRead(RESET_PIN) == LOW) {
        resetConfig();
    }

    prefs.begin(PREF_NAMESPACE, false);
    savedSSID = prefs.getString(KEY_SSID, "");
    savedPass = prefs.getString(KEY_PASS, "");
    savedApiKey = prefs.getString(KEY_APIKEY, "");
    savedPhone = prefs.getString(KEY_PHONE, "");

    Serial.println("Saved config:");
    Serial.printf("SSID: %s | API: %s | Phone: %s\n", savedSSID.c_str(), savedApiKey.c_str(), savedPhone.c_str());

    if (savedSSID.length() && savedApiKey.length() && savedPhone.length()) {
        if (connectToWiFi()) {
            inCaptivePortal = false;
            return true;
        }
        Serial.println("⚠️ Wi-Fi connect failed, opening portal...");
    }

    startCaptivePortal();
    return false;
}

/*============================
  LOOP HANDLER
=============================*/
void handleWiFiPortalLoop() {
    if (inCaptivePortal) {
        dnsServer.processNextRequest();
        server.handleClient();
    }
}

String getSavedApiKey() { return savedApiKey; }
String getSavedPhone()  { return savedPhone;  }