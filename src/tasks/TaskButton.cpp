#include "TaskButton.h"

extern QueueHandle_t xQueueButton;

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

void TaskButton(void *pvParameters) {
    (void) pvParameters;

    pinMode(BUTTON_START, INPUT_PULLUP);
    pinMode(BUTTON_DETECT, INPUT_PULLUP);
    pinMode(BUTTON_SPEAKER, INPUT_PULLUP);

    uint8_t buttonStateCount = 0;
    uint8_t buttonDetectStateCount = 0;
    uint8_t buttonSpeakerStateCount = 0;

    while (1) {
        bool readStart   = ButtonPressed(btnStart, debounceDelay);
        bool readDetect  = ButtonPressed(btnDetect, debounceDelay);
        bool readSpeaker = ButtonPressed(btnSpeaker, debounceDelay);

        // state start button = 0 or 1
        if (readStart) {
            buttonStateCount = (buttonStateCount + 1) % 2;
            Serial.println("start");
            uint8_t msgStartButton = buttonStateCount;
            xQueueSend(xQueueButton, &msgStartButton, portMAX_DELAY);
        }

        // state detect button = 2 or 3
        if (readDetect) {
            buttonDetectStateCount = (buttonDetectStateCount % 2) + 2;
            Serial.println("Detect");
            uint8_t msgDetectButton = buttonDetectStateCount;
            xQueueSend(xQueueButton, &msgDetectButton, portMAX_DELAY);
        }

        // state speaker button = 4 or 5;
        if (readSpeaker) {
            buttonSpeakerStateCount = (buttonSpeakerStateCount % 2) + 4;
            Serial.println("speaker");
            uint8_t msgSpeakerButton = buttonSpeakerStateCount;
            xQueueSend(xQueueButton, &msgSpeakerButton, portMAX_DELAY);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void createTaskButton() {
  xTaskCreatePinnedToCore(TaskButton, "Button", 4096, NULL, 2, NULL, 0);
}
