#include "WiFi_Manager.h"

void WiFiManager::connect() {
#if CFG_MODULE_WIFI == ENABLED
  
    if (WiFi.status() == WL_CONNECTED) return;

    Serial.println("WiFi_Mgr: Resetting WiFi state...");

   
    WiFi.disconnect(true);  
    WiFi.mode(WIFI_OFF);    
    vTaskDelay(pdMS_TO_TICKS(100)); 

    
    WiFi.mode(WIFI_STA);
    WiFi.persistent(false); 
    
  
    Serial.println("WiFi_Mgr: Sending connection request...");
    WiFi.begin(CFG_WIFI_SSID, CFG_WIFI_PASS);
    
    
#endif
}

bool WiFiManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED);
}