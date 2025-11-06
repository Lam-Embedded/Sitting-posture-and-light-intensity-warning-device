#include "TaskButton.h"

extern QueueHandle_t xQueueButton;

uint8_t debounceDelay = 50;

bool ButtonPressed(uint8_t button, unsigned long debounceTime) {
    static uint8_t buttonState = HIGH;
    static uint8_t lastButtonState = HIGH;
    static unsigned long lastDebounceTime = 0;
    static bool buttonPressed = false;

    uint8_t readStatus = digitalRead(button);

    if (readStatus != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceTime) {
        if (readStatus != buttonState) {
            buttonState = readStatus;
            if (buttonState == HIGH) {
                // do something
                buttonPressed = true;
            }
        }
    }
    lastButtonState = readStatus;

    if (buttonPressed) {
        buttonPressed = false;
        return true;
    }

    return false;
}

void TaskButton(void *pvParameters) {
    (void) pvParameters;
    pinMode(BUTTON_START, INPUT_PULLUP);
    pinMode(BUTTON_DETECT, INPUT_PULLUP);
    pinMode(BUTTON_SPEAKER, INPUT_PULLUP);

    while (1) {
        bool readStart = ButtonPressed(BUTTON_START, debounceDelay);
        bool readDetect = ButtonPressed(BUTTON_DETECT, debounceDelay);
        bool readSpeaker = ButtonPressed(BUTTON_SPEAKER, debounceDelay);

        if (readStart || readDetect || readSpeaker) {
            uint8_t msg = 1;
            xQueueSend(xQueueButton, &msg, portMAX_DELAY);
        }
    }
}

void createTaskButton() {
  xTaskCreatePinnedToCore(TaskButton, "Button", 2048, NULL, 2, NULL, 0);
}
