#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("Đang khởi tạo cảm biến VL53L0X...");
  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("Lỗi: Không tìm thấy cảm biến VL53L0X!");
    while (1);
  }
  Serial.println("Cảm biến VL53L0X đã sẵn sàng!");
}

void loop() {
  int distance = sensor.readRangeSingleMillimeters();

  if (sensor.timeoutOccurred()) {
    Serial.println("Lỗi: Cảm biến timeout!");
  } else {
    Serial.print("Khoảng cách: ");
    Serial.print(distance);
    Serial.println(" mm");
  }

  delay(500);
}
