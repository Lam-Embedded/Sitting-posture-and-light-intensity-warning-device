#include "TaskAudio.h"

extern QueueHandle_t xQueueAudio;

// ========================== AUDIO ============================
Audio audio;
File root;
String audioFiles[5];             // Tối đa 5 file
uint8_t totalFiles = 0;
uint8_t currentIndex = 0;

// func
void initSD();
void scanAudioFiles();
void initAudio();
void playCurrentFile();
void audio_info(const char* info);

void TaskAudio(void *pvParameters) {
    (void) pvParameters;
    initSD();
    scanAudioFiles();
    initAudio();

    while(1) {
        // do something

        if (audio.isRunning()) {
            audio.loop();
        }
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

void createTaskAudio() {
    xTaskCreatePinnedToCore(TaskAudio, "Audio", 4096, NULL, 3, NULL, 1);
}
