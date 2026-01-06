#include "Telegram_Manager.h"

#if CFG_MODULE_TELEGRAM == ENABLED

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(CFG_TG_BOT_TOKEN, clientTCP);


uint8_t* global_jpg_ptr = NULL;
size_t global_jpg_len = 0;
size_t global_jpg_idx = 0;


bool isDataAvailable() {
    return (global_jpg_ptr != NULL && global_jpg_idx < global_jpg_len);
}

byte getNextByteData() {
    if (global_jpg_ptr && global_jpg_idx < global_jpg_len) {
        return global_jpg_ptr[global_jpg_idx++];
    }
    return 0;
}

void TelegramManager::init() {
    clientTCP.setInsecure();
}

String TelegramManager::getRiskLevel(int index) {
    switch(index) {
        case 0: return "Safe";
        case 1: case 3: case 7: case 8: return "🚨 HIGH DANGER";
        case 2: case 4: case 5: case 6: case 9: return "⚠️ Warning";
        default: return "Unknown";
    }
}

void TelegramManager::sendIPG(int index, String label, float score, uint8_t* jpg_buf, size_t jpg_len) {
    
    Serial.printf("Telegram: Sending Image (%d bytes)...\n", jpg_len);

   
    global_jpg_ptr = jpg_buf;
    global_jpg_len = jpg_len;
    global_jpg_idx = 0;

    String response = bot.sendPhotoByBinary(
        CFG_TG_CHAT_ID,
        "image/jpeg",
        jpg_len,
        isDataAvailable,
        getNextByteData,
        nullptr,
        nullptr
    );

    if (response.length() > 0) {
        Serial.println("Telegram: Photo Sent! Sending Caption...");
        String msg = "Status: " + label + "\nRisk: " + getRiskLevel(index) + "\nConf: " + String(score, 1) + "%";
        bot.sendMessage(CFG_TG_CHAT_ID, msg, "");
    } else {
        Serial.println("Telegram: Failed ❌");
    }
}

#endif