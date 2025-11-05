#include "TaskAudio.h"

void TaskAudio(void *pvParameters) {
    (void) pvParameters;
}

void createTaskAudio() {
    xTaskCreatePinnedToCore(TaskAudio, "Audio", 4096, NULL, 3, NULL, 1);
}
