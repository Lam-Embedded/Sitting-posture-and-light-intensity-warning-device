#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

#define RESET_PIN 12

const char* AP_SSID = "Device-Setup";
const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress NET_MASK(255, 255, 255, 0);
const byte DNS_PORT = 53;

WebServer server(80);
DNSServer dnsServer;
Preferences prefs;

// Keys trong Preferences
const char* PREF_NAMESPACE = "config";
const char* KEY_SSID = "wifi_ssid";
const char* KEY_PASS = "wifi_pass";
const char* KEY_APIKEY = "apikey";
const char* KEY_PHONE = "phone";

String savedSSID, savedPass, savedApiKey, savedPhone;

void startCaptivePortal();
void handleRoot();
void handleSave();
void handleNotFound();
void resetConfig();
bool connectToWiFi();

void setup() {
  Serial.begin(115200);
  pinMode(RESET_PIN, INPUT_PULLUP);
  delay(300);

  // Giữ nút Boot khi bật để reset cấu hình
  if (digitalRead(RESET_PIN) == LOW) {
    resetConfig();
  }

  prefs.begin(PREF_NAMESPACE, false);
  savedSSID = prefs.getString(KEY_SSID, "");
  savedPass = prefs.getString(KEY_PASS, "");
  savedApiKey = prefs.getString(KEY_APIKEY, "");
  savedPhone = prefs.getString(KEY_PHONE, "");

  Serial.println("\n--- Device Boot ---");
  Serial.printf("Saved SSID: '%s'\n", savedSSID.c_str());
  Serial.printf("Saved API Key: '%s'\n", savedApiKey.c_str());
  Serial.printf("Saved Phone: '%s'\n", savedPhone.c_str());

  // Nếu có đủ thông tin, thử kết nối WiFi
  if (savedSSID.length() && savedApiKey.length() && savedPhone.length()) {
    if (connectToWiFi()) {
      Serial.println("✅ Connected! IP: " + WiFi.localIP().toString());
      // do something

    } else {
      Serial.println("⚠️ Failed to connect, switching to captive portal...");
    }
  }

  startCaptivePortal();
}

void loop() {
  if (WiFi.getMode() == WIFI_AP) {
    dnsServer.processNextRequest();
    server.handleClient();
  }
}

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

  if (WiFi.getMode() == WIFI_AP) {
    dnsServer.start(DNS_PORT, "*", AP_IP);
  }

  // Route cơ bản
  server.on("/", HTTP_GET, handleRoot);
  server.on("/generate_204", HTTP_GET, handleRoot);         // Android captive portal check
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot);  // Apple captive portal check
  server.on("/favicon.ico", HTTP_GET, []() { server.send(200, "text/plain", ""); });
  server.on("/save", HTTP_POST, handleSave);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.printf("🌐 Captive portal started on AP '%s'\n", AP_SSID);
  Serial.println("Connect your phone and it will auto-open setup page.");
}

/*============================
  WEB PAGE HANDLERS
=============================*/
void handleRoot() {
  String page = "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>Device Setup</title>";
  page += "<style>body{font-family:Arial;padding:20px;}label{display:block;margin-top:10px;}input{width:100%;padding:8px;}button{margin-top:15px;padding:10px;width:100%;}</style>";
  page += "</head><body><h2>ESP32 Configuration</h2>";
  page += "<form method='POST' action='/save'>";
  page += "<label>Wi-Fi SSID</label><input name='ssid' placeholder='Wi-Fi Name' required>";
  page += "<label>Wi-Fi Password</label><input name='pass' type='password' placeholder='Wi-Fi Password'>";
  page += "<label>API Key</label><input name='apikey' placeholder='Your API Key' required>";
  page += "<label>Phone Number (e.g. 84901234567)</label><input name='phone' placeholder='Phone Number' required>";
  page += "<button type='submit'>Save & Connect</button></form>";
  page += "<p style='color:#666;font-size:0.9em'>Hold BOOT button when starting device to reset configuration.</p>";
  page += "</body></html>";

  server.send(200, "text/html", page);
}

void handleSave() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  String apikey = server.arg("apikey");
  String phone = server.arg("phone");

  Serial.println("\n📥 Received config:");
  Serial.println("SSID: " + ssid);
  Serial.println("API Key: " + apikey);
  Serial.println("Phone: " + phone);

  prefs.putString(KEY_SSID, ssid);
  prefs.putString(KEY_PASS, pass);
  prefs.putString(KEY_APIKEY, apikey);
  prefs.putString(KEY_PHONE, phone);

  String page = "<!doctype html><html><body>";
  page += "<h3>Saving configuration...</h3><p>Connecting to Wi-Fi...</p></body></html>";
  server.send(200, "text/html", page);

  dnsServer.stop();
  server.stop();
  delay(500);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to new Wi-Fi...");

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ Wi-Fi Connected! IP: " + WiFi.localIP().toString());
    delay(2000);
    ESP.restart();
  } else {
    Serial.println("❌ Failed to connect. Returning to config mode...");
    startCaptivePortal();
  }
}

void handleNotFound() {
  server.sendHeader("Location", String("http://") + AP_IP.toString(), true);
  server.send(302, "text/plain", "");
}
