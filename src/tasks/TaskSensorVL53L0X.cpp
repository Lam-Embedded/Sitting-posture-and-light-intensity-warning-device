#include "TaskSensorVL53L0X.h"

void TaskSensorVL53L0XLaser(void *pvParameters) {
  (void) pvParameters;
  // code something
}

void createTaskVL53L0X() {
  xTaskCreatePinnedToCore(TaskSensorVL53L0XLaser, "VL53L0X", 2048, NULL, 2, NULL, 0);
}
