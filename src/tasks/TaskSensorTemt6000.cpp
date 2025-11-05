#include "TaskSensorTemt6000.h"

void TaskSensorTemt6000(void *pvParameters) {
    (void) pvParameters;
}

void createTaskTemt6000() {
    xTaskCreatePinnedToCore(TaskSensorTemt6000, "Temt6000", 2048, NULL, 2, NULL, 1);
}
