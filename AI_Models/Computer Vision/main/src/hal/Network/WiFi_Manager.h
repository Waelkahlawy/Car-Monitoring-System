#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "../../Config.h"

class WiFiManager {
public:
    static void connect();
    static bool isConnected();
};
#endif