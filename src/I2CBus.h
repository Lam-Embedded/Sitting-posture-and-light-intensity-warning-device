// I2CBUS.h
#pragma once
#include <Arduino.h>
#include <Wire.h>

extern SemaphoreHandle_t xI2CMutex;

void initI2CBus();
