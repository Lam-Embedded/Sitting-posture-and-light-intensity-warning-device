#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter(0x23);

void setup() {
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(400000);  // Fast I2C

  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Failed to init BH1750");
    while(1) vTaskDelay(1000);
  }
}

void loop() {

  float lux = lightMeter.readLightLevel();

  if (lux > 900 || lux < 200) {
    Serial.println("Loi anh sang");
  }

  delay(120);   // match measurement time
}
