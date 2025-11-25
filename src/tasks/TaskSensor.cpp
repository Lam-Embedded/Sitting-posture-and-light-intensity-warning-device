#include "TaskSensor.h"

extern QueueHandle_t xQueueSensor;

VL53L0X sensorToF;
BH1750 lightMeter;
uint8_t msg = 0;

static bool running = false;
uint8_t mode = 0;
uint8_t debounceDelay = 50;

struct ButtonState {
  uint8_t pin;
  uint8_t buttonState;
  uint8_t lastButtonState;
  unsigned long lastDebounceTime;
  bool buttonPressed;
};

ButtonState btnStart   = {BUTTON_START, HIGH, HIGH, 0, false};
ButtonState btnDetect  = {BUTTON_DETECT, HIGH, HIGH, 0, false};
ButtonState btnSpeaker = {BUTTON_SPEAKER, HIGH, HIGH, 0, false};

bool ButtonPressed(ButtonState &btn, unsigned long debounceTime);
void readSensor();
void notificationMode(uint8_t mode_setup);

void TaskSensor(void *pvParameters) {
    (void) pvParameters;

        // Khởi động I2C
    Wire.begin(21, 22);   // SDA, SCL (tùy board, có thể bỏ vì ESP32 mặc định là 21/22)

    // ---- VL53L0X ----
    sensorToF.setTimeout(500);
    if (!sensorToF.init()) {
        Serial.println("Failed to detect VL53L0X!");
        while (1);
    }
    sensorToF.startContinuous();

    // ---- BH1750 ----
    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        Serial.println("Failed to detect BH1750!");
        vTaskDelete(NULL);
    }

    Serial.println("Sensors OK");


    while (1) {
        bool readStart   = ButtonPressed(btnStart, debounceDelay);
        bool readDetect  = ButtonPressed(btnDetect, debounceDelay);
        bool readSpeaker = ButtonPressed(btnSpeaker, debounceDelay);

        if (readStart) {
            running = !running;
        }

        if (running) {
            if (readDetect) {
                mode++;
                mode = (mode > 3) ? 0 : mode;
                Serial.println(mode);
            }
            readSensor();
        }

        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

void createTaskSensor() {  
    xTaskCreatePinnedToCore(TaskSensor, "sensor", 4096, NULL, 2, NULL, 1);
}

void notificationMode(uint8_t mode_setup) {
    if (mode_setup == 0) {
        xQueueSend(xQueueSensor, &msg, portMAX_DELAY);
        tone(SPEAKER, 5000, 30);
    }
    else if (mode_setup == 1) {
        tone(SPEAKER, 5000, 30);
    }
    else {
        xQueueSend(xQueueSensor, &msg, portMAX_DELAY);
        noTone(SPEAKER);
    }
}

void readSensor() {
    // Đọc VL53L0X
    int distance = sensorToF.readRangeContinuousMillimeters();
    if (sensorToF.timeoutOccurred()) {
        Serial.println("VL53 TIMEOUT");
    }
    // Đọc BH1750
    float lux = lightMeter.readLightLevel();
    // Print
    // Serial.print("Distance: ");
    // Serial.print(distance);
    // Serial.print(" mm   |   Lux: ");
    // Serial.print(lux);
    // Serial.println(" lx");
    if (distance < 200 || distance > 600) {
        msg = 1;
        notificationMode(mode);
    }
    else if (lux < 200 || lux > 900) {
        msg = 2;
        notificationMode(mode);
    }
    else {
        msg = 0;
        xQueueSend(xQueueSensor, &msg, portMAX_DELAY);
    }
}

bool ButtonPressed(ButtonState &btn, unsigned long debounceTime) {
    uint8_t readStatus = digitalRead(btn.pin);

    if (readStatus != btn.lastButtonState) {
        btn.lastDebounceTime = millis();
    }

    if ((millis() - btn.lastDebounceTime) > debounceTime) {
        if (readStatus != btn.buttonState) {
            btn.buttonState = readStatus;
            if (btn.buttonState == HIGH) {
                btn.buttonPressed = true;
                tone(SPEAKER, 2000, 50);
            }
        }
    }

    btn.lastButtonState = readStatus;

    if (btn.buttonPressed) {
        btn.buttonPressed = false;
        return true;
    }
    return false;
}