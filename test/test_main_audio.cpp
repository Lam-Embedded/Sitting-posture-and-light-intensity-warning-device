#include <Arduino.h>

#include "tasks/TaskAudio.h"

void initTasks() {

    createTaskAudio();

}
void setup() {
    Serial.begin(115200);
    delay(200);
    initTasks();
}

void loop() {

}
