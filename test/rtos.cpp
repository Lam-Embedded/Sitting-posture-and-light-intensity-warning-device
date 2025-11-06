#include <Arduino.h>

// ============================= CẤU HÌNH =============================
#define LED_PIN 2

// Tạo handle cho Queue
QueueHandle_t ledQueue;

// ============================= TASK 1 =============================
// Task nháy LED và gửi trạng thái qua queue
void TaskBlink(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  bool ledState = false;

  while (1) {
    // Đổi trạng thái LED
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);

    // Gửi trạng thái vào queue
    if (xQueueSend(ledQueue, &ledState, portMAX_DELAY) == pdPASS) {
      // Gửi thành công
    }

    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1s
  }
}

// ============================= TASK 2 =============================
// Task nhận trạng thái LED và in ra Serial
void TaskMonitor(void *pvParameters) {
  bool receivedState;

  while (1) {
    // Nhận dữ liệu từ queue (chờ mãi tới khi có dữ liệu)
    if (xQueueReceive(ledQueue, &receivedState, portMAX_DELAY) == pdPASS) {
      Serial.print("LED state: ");
      Serial.println(receivedState ? "ON" : "OFF");
    }
  }
}

// ============================= SETUP =============================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Khởi tạo queue: chứa tối đa 5 phần tử, mỗi phần tử là kiểu bool
  ledQueue = xQueueCreate(5, sizeof(bool));

  if (ledQueue == NULL) {
    Serial.println("Không tạo được queue!");
    while (1);
  }

  // Tạo task nháy LED (core 0)
  xTaskCreatePinnedToCore(TaskBlink, "Blink Task", 2048, NULL, 1, NULL, 0);

  // Tạo task in trạng thái (core 1)
  xTaskCreatePinnedToCore(TaskMonitor, "Monitor Task", 2048, NULL, 1, NULL, 1);
}

void loop() {
  // Không dùng loop() trong FreeRTOS
}
