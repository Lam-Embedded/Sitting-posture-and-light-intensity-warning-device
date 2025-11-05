#include <Arduino.h>
#include <WiFi.h>
#include <Audio.h>
#include <SD.h>
#include <FS.h>
#include "setUpWifi.h"

// ==================== PIN DEFINE =====================
const uint8_t BUTTON_START = 15;
const uint8_t BUTTON_STOP  = 2;
const uint8_t BUTTON_CHANGE = 4;
const uint8_t SPEAKER_PIN = 34;

// ==================== AUDIO VARIABLES =================
Audio audio;
File root;
String audioFiles[5];
uint8_t totalFiles = 0;
uint8_t currentIndex = 0;

// ==================== BUTTON STATE ====================
uint8_t buttonChangeState = HIGH;
uint8_t lastButtonChangeState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

volatile bool systemReady = false;
volatile uint8_t countChange = 0;

// ==================== FREE RTOS ====================
QueueHandle_t xQueueButton;
QueueHandle_t xQueueAudio;

enum ButtonEvent {
  BTN_NONE,
  BTN_START,
  BTN_STOP,
  BTN_CHANGE
};

enum AudioCommand {
  AUDIO_NONE,
  AUDIO_PLAY_FILE,
  AUDIO_STOP
};

// ==================== FUNCTION DECLARATION ====================
void initSD();
void scanAudioFiles();
void initAudio();
void playCurrentFile();
void stopSystem();
void audio_info(const char* info);

// Task prototypes
void TaskButton(void *pvParameters);
void TaskLogic(void *pvParameters);
void TaskAudio(void *pvParameters);

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== FreeRTOS Audio Project ===");

  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(BUTTON_STOP, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(BUTTON_CHANGE, INPUT_PULLUP);

  // SD & Audio setup
  initSD();
  scanAudioFiles();
  initAudio();

  // Queue
  xQueueButton = xQueueCreate(5, sizeof(ButtonEvent));
  xQueueAudio  = xQueueCreate(5, sizeof(AudioCommand));

  // Task
  xTaskCreate(TaskButton, "Button", 2048, NULL, 2, NULL);
  xTaskCreate(TaskLogic,  "Logic",  3072, NULL, 3, NULL);
  xTaskCreate(TaskAudio,  "Audio",  4096, NULL, 2, NULL);

  attachInterrupt(digitalPinToInterrupt(BUTTON_STOP), stopSystem, CHANGE);
}

// ==================== LOOP (Không dùng) ====================
void loop() {}

// ==================== TASK BUTTON ====================
void TaskButton(void *pvParameters) {
  ButtonEvent evt = BTN_NONE;

  for (;;) {
    int startState  = digitalRead(BUTTON_START);
    int changeState = digitalRead(BUTTON_CHANGE);

    // --- START pressed ---
    if (startState == LOW) {
      vTaskDelay(pdMS_TO_TICKS(50));
      if (digitalRead(BUTTON_START) == LOW) {
        evt = BTN_START;
        xQueueSend(xQueueButton, &evt, 0);
        digitalWrite(SPEAKER_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(10));
        digitalWrite(SPEAKER_PIN, LOW);
      }
    }

    // --- CHANGE pressed (debounce) ---
    if (changeState != lastButtonChangeState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (changeState != buttonChangeState) {
        buttonChangeState = changeState;
        if (buttonChangeState == HIGH) {
          evt = BTN_CHANGE;
          xQueueSend(xQueueButton, &evt, 0);
          digitalWrite(SPEAKER_PIN, HIGH);
          vTaskDelay(pdMS_TO_TICKS(10));
          digitalWrite(SPEAKER_PIN, LOW);
        }
      }
    }
    lastButtonChangeState = changeState;

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// ==================== TASK LOGIC ====================
void TaskLogic(void *pvParameters) {
  ButtonEvent btn;
  AudioCommand cmd;

  for (;;) {
    if (xQueueReceive(xQueueButton, &btn, portMAX_DELAY)) {
      switch (btn) {
        case BTN_START:
          systemReady = true;
          Serial.println("[LOGIC] System started");
          cmd = AUDIO_PLAY_FILE;
          xQueueSend(xQueueAudio, &cmd, 0);
          break;

        case BTN_CHANGE:
          if (systemReady) {
            countChange++;
            if (countChange > 3) countChange = 0;
            Serial.printf("[LOGIC] Mode changed to %d\n", countChange);
          }
          break;

        case BTN_STOP:
          systemReady = false;
          countChange = 0;
          cmd = AUDIO_STOP;
          xQueueSend(xQueueAudio, &cmd, 0);
          Serial.println("[LOGIC] System stopped");
          break;

        default:
          break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// ==================== TASK AUDIO ====================
void TaskAudio(void *pvParameters) {
  AudioCommand cmd;

  for (;;) {
    if (xQueueReceive(xQueueAudio, &cmd, portMAX_DELAY)) {
      switch (cmd) {
        case AUDIO_PLAY_FILE:
          playCurrentFile();
          break;

        case AUDIO_STOP:
          audio.stopSong();
          Serial.println("[AUDIO] Stopped");
          break;

        default:
          break;
      }
    }

    // FreeRTOS cho phép audio.loop() chạy đều
    if (audio.isRunning()) {
      audio.loop();
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// ==================== INTERRUPT ====================
void stopSystem() {
  systemReady = false;
  countChange = 0;
  AudioCommand cmd = AUDIO_STOP;
  xQueueSendFromISR(xQueueAudio, &cmd, NULL);
}

// ==================== AUDIO FUNCTIONS ====================
void initSD() {
  Serial.print("[INFO] Initializing SD card... ");
  if (!SD.begin(SD_CS)) {
    Serial.println("FAILED!");
    while (true);
  }
  Serial.println("OK");
}

void scanAudioFiles() {
  Serial.println("[INFO] Scanning SD card...");
  root = SD.open("/");
  if (!root) {
    Serial.println("[ERROR] Cannot open SD root!");
    while (true);
  }

  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    String name = entry.name();

    if (!entry.isDirectory() && name.endsWith(".mp3")) {
      audioFiles[totalFiles++] = name;
      Serial.printf("  Found: %s\n", name.c_str());
    }
    entry.close();
  }

  if (totalFiles == 0) {
    Serial.println("[ERROR] No MP3 files found!");
    while (true);
  }
  Serial.printf("[INFO] Found %d audio files.\n", totalFiles);
}

void initAudio() {
  Serial.println("[INFO] Setting up I2S...");
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(255);
  // audio.setInfoCallback(audio_info);
}

void playCurrentFile() {
  String filePath = "/" + audioFiles[currentIndex];
  Serial.printf("[PLAY] %s\n", filePath.c_str());
  audio.stopSong();
  audio.connecttoFS(SD, filePath.c_str());
}

void audio_info(const char* info) {
  Serial.print("[AUDIO] ");
  Serial.println(info);
}
