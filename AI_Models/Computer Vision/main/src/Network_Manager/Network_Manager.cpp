#include "Network_Manager.h"

// تحديث الاسم هنا أيضاً
WiFiServer AppNetworkManager::jsonServer(CFG_API_PORT);

void AppNetworkManager::connect() {
    WiFi.begin(CFG_WIFI_SSID, CFG_WIFI_PASS);
    Serial.print("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
}

void AppNetworkManager::startServers() {
    startCameraServer(); // From app_httpd.cpp
    jsonServer.begin();
}

void AppNetworkManager::handleRequests(const char* label, float confidence) {
    WiFiClient client = jsonServer.available();
    if (client) {
        while (client.connected() && client.available()) client.read();
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Access-Control-Allow-Origin: *");
        client.println("Connection: close");
        client.println();
        
        client.print("{\"label\":\""); client.print(label);
        client.print("\",\"score\":"); client.print(confidence * 100.0, 1);
        client.println("}");
        client.stop();
    }
}