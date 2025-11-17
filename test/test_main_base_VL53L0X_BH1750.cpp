#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <BH1750.h>

VL53L0X sensorToF;
BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  pinMode(SPEAKER, OUTPUT);

  // Khởi động I2C
  Wire.begin(21, 22);   // SDA, SCL (tùy board, có thể bỏ vì ESP32 mặc định là 21/22)

  // ---- VL53L0X ----
  sensorToF.setTimeout(500);
  if (!sensorToF.init()) {
    Serial.println("Failed to detect VL53L0X!");
    while (1);
  }
  sensorToF.startContinuous();

  // ---- BH1750 ----
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Failed to detect BH1750!");
    while (1);
  }

  Serial.println("Sensors OK");
}

void loop() {
  // Đọc VL53L0X
  int distance = sensorToF.readRangeContinuousMillimeters();
  if (sensorToF.timeoutOccurred()) {
    Serial.println("VL53 TIMEOUT");
  }

  // Đọc BH1750
  float lux = lightMeter.readLightLevel();

  // Print
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" mm   |   Lux: ");
  Serial.print(lux);
  Serial.println(" lx");

  delay(200);
}