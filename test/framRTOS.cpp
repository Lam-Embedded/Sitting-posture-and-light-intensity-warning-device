#include <Arduino.h>

// =========================================================
// KHAI BÁO CÁC TASK
// =========================================================
void TaskBlink(void *pvParameters);
void TaskButton(void *pvParameters);
void TaskSerial(void *pvParameters);

// =========================================================
// CÁC BIẾN TOÀN CỤC / CẤU HÌNH
// =========================================================
const int LED_PIN = 2;
const int BUTTON_PIN = 15;
volatile bool ledState = false;

// =========================================================
// SETUP
// =========================================================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Tạo các task
  xTaskCreatePinnedToCore(
    TaskBlink,        // Hàm task
    "Blink Task",     // Tên task
    2048,             // Stack size (byte)
    NULL,             // Tham số
    1,                // Độ ưu tiên (cao hơn số lớn hơn)
    NULL,             // Handle
    1                 // Core 0 hoặc 1
  );

  xTaskCreatePinnedToCore(
    TaskButton,
    "Button Task",
    2048,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    TaskSerial,
    "Serial Task",
    2048,
    NULL,
    1,
    NULL,
    0
  );

  Serial.println("=== FreeRTOS Example Started ===");
}

// =========================================================
// LOOP (KHÔNG DÙNG NẾU CHẠY TOÀN BỘ QUA TASK)
// =========================================================
void loop() {
  // Không cần code ở đây — mọi thứ do FreeRTOS quản lý
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// =========================================================
// CÁC TASK
// =========================================================

// Task nhấp nháy LED
void TaskBlink(void *pvParameters) {
  while (1) {
    digitalWrite(LED_PIN, ledState);
    vTaskDelay(200 / portTICK_PERIOD_MS);  // Delay 200ms
  }
}

// Task đọc nút nhấn
void TaskButton(void *pvParameters) {
  static bool lastState = HIGH;
  while (1) {
    bool current = digitalRead(BUTTON_PIN);
    if (current == LOW && lastState == HIGH) {
      ledState = !ledState;
    }
    lastState = current;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

// Task gửi log Serial
void TaskSerial(void *pvParameters) {
  while (1) {
    Serial.print("LED State: ");
    Serial.println(ledState ? "ON" : "OFF");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
