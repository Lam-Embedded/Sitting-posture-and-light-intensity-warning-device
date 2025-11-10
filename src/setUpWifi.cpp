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
bool isFirstSetup = false;

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
        String page = R"rawliteral(
        <!DOCTYPE html>
        <html lang='en'>
        <head>
            <meta charset='UTF-8'>
            <meta name='viewport' content='width=device-width, initial-scale=1.0'>
            <title>ESP32 Wi-Fi Setup</title>
            <style>
                * { box-sizing: border-box; }
                body {
                    font-family: 'Segoe UI', Tahoma, sans-serif;
                    background: linear-gradient(135deg, #74ABE2, #5563DE);
                    margin: 0; padding: 0;
                    display: flex; align-items: center; justify-content: center;
                    height: 100vh; color: #333;
                }
                .card {
                    background: #fff;
                    border-radius: 16px;
                    box-shadow: 0 8px 20px rgba(0,0,0,0.15);
                    width: 90%;
                    max-width: 420px;
                    padding: 28px;
                    animation: fadeIn 0.5s ease;
                }
                h2 {
                    text-align: center;
                    color: #445;
                    margin-bottom: 20px;
                }
                label {
                    font-weight: 600;
                    display: block;
                    margin: 12px 0 6px;
                }
                select, input {
                    width: 100%;
                    padding: 10px 12px;
                    border: 1px solid #ccc;
                    border-radius: 8px;
                    font-size: 14px;
                    transition: all 0.2s ease;
                }
                input:focus, select:focus {
                    border-color: #5563DE;
                    outline: none;
                    box-shadow: 0 0 0 2px rgba(85,99,222,0.2);
                }
                button {
                    width: 100%;
                    background: #5563DE;
                    color: white;
                    border: none;
                    padding: 12px;
                    border-radius: 8px;
                    font-size: 15px;
                    font-weight: bold;
                    cursor: pointer;
                    margin-top: 18px;
                    transition: all 0.2s ease;
                }
                button:hover {
                    background: #3F4CC3;
                    transform: translateY(-1px);
                }
                .loading {
                    text-align: center;
                    color: #666;
                    font-size: 0.9em;
                    margin: 8px 0 16px;
                }
                p.note {
                    text-align: center;
                    font-size: 0.85em;
                    color: #777;
                    margin-top: 15px;
                }
                @keyframes fadeIn {
                    from { opacity: 0; transform: translateY(10px); }
                    to { opacity: 1; transform: translateY(0); }
                }
            </style>
        </head>
        <body>
            <div class='card'>
                <h2>🔧 ESP32 Wi-Fi Setup</h2>
                <form method='POST' action='/save'>
                    <label>Select Wi-Fi</label>
                    <select id='wifiList' required>
                        <option value=''>🔍 Scanning...</option>
                    </select>
                    <input id='ssidInput' name='ssid' type='hidden'>

                    <label>Password</label>
                    <input name='pass' type='password' placeholder='Enter password'>

        )rawliteral";

        if (isFirstSetup) {
            page += R"rawliteral(
                    <label>API Key</label>
                    <input name='apikey' required placeholder='Enter API key'>
                    <label>Phone Number</label>
                    <input name='phone' required placeholder='Enter phone number'>
            )rawliteral";
        } else {
            page += "<input type='hidden' name='apikey' value='" + savedApiKey + "'>";
            page += "<input type='hidden' name='phone' value='" + savedPhone + "'>";
        }

        page += R"rawliteral(
                    <button type='submit'>Save & Connect</button>
                </form>
                <p class='note'>Hold <b>BOOT</b> to reset configuration</p>
            </div>

            <script>
                async function loadWiFiList() {
                    try {
                        const res = await fetch('/scan');
                        const data = await res.json();
                        const list = document.getElementById('wifiList');
                        list.innerHTML = '<option value=\"\">-- Select Wi-Fi --</option>';
                        data.forEach(ssid => {
                            const opt = document.createElement('option');
                            opt.value = ssid;
                            opt.textContent = ssid;
                            list.appendChild(opt);
                        });
                        list.addEventListener('change', () => {
                            document.getElementById('ssidInput').value = list.value;
                        });
                    } catch (err) {
                        document.getElementById('wifiList').innerHTML = '<option>Error scanning</option>';
                    }
                }
                loadWiFiList();
            </script>
        </body>
        </html>
        )rawliteral";

        server.send(200, "text/html", page);
    });

    server.on("/scan", HTTP_GET, []() {
        int n = WiFi.scanNetworks();
        String json = "[";
        for (int i = 0; i < n; ++i) {
            if (i) json += ",";
            json += "\"" + WiFi.SSID(i) + "\"";
        }
        json += "]";
        server.send(200, "application/json", json);
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

        String html = R"rawliteral(
        <!DOCTYPE html>
        <html lang='en'>
        <head>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>
        <title>Saving...</title>
        <style>
            body {
            font-family: 'Segoe UI', sans-serif;
            background: linear-gradient(135deg,#74ABE2,#5563DE);
            color:white;display:flex;flex-direction:column;
            align-items:center;justify-content:center;
            height:100vh;text-align:center;margin:0;
            }
            .loader{border:6px solid rgba(255,255,255,0.3);
            border-top:6px solid #fff;border-radius:50%;
            width:60px;height:60px;animation:spin 1s linear infinite;margin-bottom:20px;}
            @keyframes spin{100%{transform:rotate(360deg);}}
            h2{margin-bottom:8px;}
            p{color:rgba(255,255,255,0.9);font-size:0.95em;}
        </style>
        </head>
        <body>
        <div class='loader'></div>
        <h2>Saving your settings...</h2>
        <p>Connecting to Wi-Fi network...<br>Devices will restart shortly.</p>
        <p id='msg' style='margin-top:12px;font-size:0.9em;opacity:0.8'>
            You can close this window after 5 seconds.
        </p>
        <script>
            setTimeout(()=>{
            document.getElementById('msg').innerText = 'If this page stays, please close it manually.';
            document.title = 'Restarting...';
            },5000);
        </script>
        </body>
        </html>
        )rawliteral";

        server.send(200, "text/html", html);
        delay(1200);
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

    if (savedApiKey.length() && savedPhone.length()) {
        if (savedSSID.length()) {
            if (connectToWiFi()) {
                inCaptivePortal = false;
                return true;
            }
            Serial.println("⚠️ Wi-Fi connect failed, opening Wi-Fi setup portal...");
            // Không cần nhập lại API và Phone
            isFirstSetup = false;
            startCaptivePortal();
            return false;
        } else {
            Serial.println("⚠️ Chưa có Wi-Fi, mở portal nhập Wi-Fi...");
            isFirstSetup = false;
            startCaptivePortal();
            return false;
        }
    } else {
        Serial.println("🆕 Lần đầu thiết lập, yêu cầu nhập đầy đủ thông tin...");
        isFirstSetup = true;
        startCaptivePortal();
        return false;
    }

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