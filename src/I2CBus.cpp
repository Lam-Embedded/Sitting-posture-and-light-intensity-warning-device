#include "I2CBus.h"

SemaphoreHandle_t xI2CMutex = NULL;

void initI2CBus() {
    if (xI2CMutex == NULL) {
        xI2CMutex = xSemaphoreCreateMutex();
    }

    Wire.begin();
    Wire.setClock(400000);   // Fast Mode 400kHz
}
