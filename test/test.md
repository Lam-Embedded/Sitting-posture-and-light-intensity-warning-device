// send data
uint8_t msg = buttonStateCount;
xQueueSend(xQueueButton, &msg, portMAX_DELAY);
// nhan data
uint8_t readVL53L0XState = 0;

if (xQueueReceive(xQueueVL53L0X, &readVL53L0XState, portMAX_DELAY) == pdTRUE) {

}