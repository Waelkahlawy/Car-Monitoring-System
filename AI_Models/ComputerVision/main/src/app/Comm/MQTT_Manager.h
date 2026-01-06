#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "../../Config.h"

#if CFG_MODULE_MQTT == ENABLED
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTManager {
public:
    static void init();
    static void loop();
    
  
    static bool publishData(const char* label, float score);
    
    
    static bool publishImage(uint8_t* image_data, size_t image_len);

private:
    static void reconnect();
};
#endif
#endif