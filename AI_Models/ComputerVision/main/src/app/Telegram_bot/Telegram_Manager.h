#ifndef TELEGRAM_MANAGER_H
#define TELEGRAM_MANAGER_H

#include "../../Config.h"

#if CFG_MODULE_TELEGRAM == ENABLED
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

class TelegramManager {
public:
    static void init();
   
    static void sendIPG(int index, String label, float score, uint8_t* jpg_buf, size_t jpg_len);

private:
    static String getRiskLevel(int index);
};
#endif
#endif