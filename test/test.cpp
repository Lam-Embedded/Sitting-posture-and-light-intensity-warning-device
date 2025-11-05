#include <Arduino.h>
#include <WiFi.h>
#include <Audio.h>
#include <SD.h>
#include <FS.h>
#include "setUpWifi.h"

// Nut nhanh
const uint8_t BUTTON_START = 15;
const uint8_t BUTTON_STOP = 2;
const uint8_t BUTTON_CHANGE = 4;
const uint8_t speaker = 34;

// Bien loc nhieu, trang thai nut nhan
uint8_t buttonChangeState = HIGH;
uint8_t lastButtonChangeState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

volatile boolean flag = false;
volatile uint8_t countChange = 0;

// ========================== AUDIO ============================
Audio audio;
File root;
String audioFiles[5];             // Tối đa 5 file
uint8_t totalFiles = 0;
uint8_t currentIndex = 0;
unsigned long lastButtonPress = 0;

// func
void initSD();
void scanAudioFiles();
void initAudio();
void playCurrentFile();
void audio_info(const char* info);

// void action (long &dist1, long &dist2, long &dist3, uint8_t countStatus);
void stopSystem();

void setup() {
    Serial.begin(115200);
    
    pinMode(speaker, OUTPUT);
    pinMode(BUTTON_STOP, INPUT_PULLUP);
    pinMode(BUTTON_START, INPUT_PULLUP);
    pinMode(BUTTON_CHANGE, INPUT_PULLUP);

    initSD();
    scanAudioFiles();
    initAudio();
    playCurrentFile();
    
    attachInterrupt(digitalPinToInterrupt(BUTTON_STOP), stopSystem, CHANGE);
    analogWrite(speaker, 0);
}

void loop() {
    if (digitalRead(BUTTON_START) == LOW) { 
        delay(50);
        if (digitalRead(BUTTON_START) == LOW) {
            flag = true;
            digitalWrite(speaker, HIGH);
            delay(10);
            digitalWrite(speaker, LOW);
        }
    }

    int reading = digitalRead(BUTTON_CHANGE);
    if (reading != lastButtonChangeState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonChangeState) {
            buttonChangeState = reading;
            
            if (buttonChangeState == HIGH) {
                countChange++;
            }
            digitalWrite(speaker, HIGH);
            delay(10);
            digitalWrite(speaker, LOW);
        }
    }
    if (countChange > 3) countChange = 0;
    lastButtonChangeState = reading;

    // Chuc nang 1
    if (flag == true && countChange == 0) {
        
    }
    // Chuc nang 2
    if (countChange == 1) {
        
    }
    // Chuc nang 3
    if (countChange == 2) {
        
    }
    // Chuc nang 4
    if (countChange == 3) {
        // action(d1, d2, d3, countChange);
    }

    if (audio.isRunning()) {
        audio.loop();
    } else {
        Serial.println("[INFO] Audio stopped. Restarting same file...");
        playCurrentFile();
    }
}

void stopSystem() {
    flag = false;
    countChange = 0;
}

void action (long &dist1, long &dist2, long &dist3, uint8_t countStatus) {
    switch (countStatus) {
    case 1:
        dist2 = 100;
        dist3 = 100;
        break;
    case 2:
        dist1 = 100;
        dist3 = 100;
        break;
    case 3:
        dist1 = 100;
        dist2 = 100;
        break;
    default:
        break;
    }
}

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
    audio.setVolume(255);  // 0–255
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
