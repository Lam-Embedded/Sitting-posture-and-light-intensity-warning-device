#include <Arduino.h>
#include <WiFi.h>
#include "setUpWifi.h"

#include "tasks/TaskSensorBH1750.h"

void initTasks() {

    createTaskBH1750();
}

void setup() {
    Serial.begin(115200);
    delay(200);
    initTasks();
}

void loop() {
    
}
