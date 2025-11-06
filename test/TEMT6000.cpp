#include <Arduino.h>

const float ADC_REF = 3.3;   // Điện áp tham chiếu ADC của ESP32
const int ADC_MAX = 4095;    // Độ phân giải 12-bit của ADC ESP32

void setup() {
  Serial.begin(115200);
}

void loop() {
  int sensorValue = analogRead(TEMT6000_PIN); // Đọc giá trị ADC
  float voltage = (sensorValue * ADC_REF) / ADC_MAX; // Quy đổi sang Volt

  // Chuyển đổi từ điện áp sang lux (xấp xỉ, phụ thuộc mạch chia điện trở)
  // Giả sử: R = 10kΩ, cảm biến TEMT6000 -> 1V ≈ 500 lux
  float lux = voltage * 500.0;

  Serial.print("ADC: ");
  Serial.print(sensorValue);
  Serial.print("  Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V  Light: ");
  Serial.print(lux, 1);
  Serial.println(" lux");

  delay(500);
}
