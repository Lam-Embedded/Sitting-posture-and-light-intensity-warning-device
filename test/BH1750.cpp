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

  if (lux == 54612) {
    Serial.println("BH1750 saturated!");
  } else if (lux < 0) {
    Serial.println("BH1750 error!");
  } else {
    Serial.printf("Light: %.1f lux\n", lux);
  }

  delay(120);   // match measurement time
}
