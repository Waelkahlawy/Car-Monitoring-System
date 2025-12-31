#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "../Config.h"
#include <WiFi.h>

// غيرنا الاسم من NetworkManager إلى AppNetworkManager
class AppNetworkManager {
public:
    static void connect();
    static void startServers();
    static void handleRequests(const char* label, float confidence);

private:
    static WiFiServer jsonServer;
};

// External declaration from app_httpd.cpp
void startCameraServer();

#endif