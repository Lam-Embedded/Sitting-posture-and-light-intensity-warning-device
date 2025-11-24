// #include "TaskSensorBH1750.h"


// BH1750 lightMeter(0x23);

// void TaskSensorBH1750(void *pvParameters) {
//     (void) pvParameters;

//     xSemaphoreTake(xI2CMutex, portMAX_DELAY);
//     bool ok = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
//     xSemaphoreGive(xI2CMutex);

//     if (!ok) {
//         Serial.println("[ERROR] BH1750 init fail");
//         vTaskDelete(NULL);
//     }

//     while (1) {

//         xSemaphoreTake(xI2CMutex, portMAX_DELAY);
//         float lux = lightMeter.readLightLevel();
//         xSemaphoreGive(xI2CMutex);

//         if (lux == 54612) {
//             Serial.println("[BH1750] Saturated!");
//         } else if (lux < 0) {
//             Serial.println("[BH1750] Error!");
//         } else {
//             Serial.printf("[BH1750] Light: %.1f lux\n", lux);
//         }

//         if (lux < 300) {
//             tone(SPEAKER, 3000, 30);
//             vTaskDelay(pdMS_TO_TICKS(50));
//         }
//         else {
//             noTone(SPEAKER);
//         }

//         vTaskDelay(pdMS_TO_TICKS(120));
//     }
// }

// void createTaskBH1750() {  
//     xTaskCreatePinnedToCore(TaskSensorBH1750, "Temt6000", 4096, NULL, 2, NULL, 1);
// }
