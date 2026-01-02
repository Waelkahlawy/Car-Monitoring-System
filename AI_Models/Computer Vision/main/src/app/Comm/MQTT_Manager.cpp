#include "MQTT_Manager.h"

#if CFG_MODULE_MQTT == ENABLED

WiFiClient espClient;
PubSubClient client(espClient);

void MQTTManager::init() {
    client.setServer(CFG_MQTT_BROKER, CFG_MQTT_PORT);
}

void MQTTManager::loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}

void MQTTManager::reconnect() {
    if (WiFi.status() == WL_CONNECTED && !client.connected()) {
        client.connect(CFG_MQTT_CLIENT_ID);
    }
}

bool MQTTManager::publish(const char* label, float score) {
    if (!client.connected()) return false;

    
    StaticJsonDocument<200> doc;
    doc["label"] = label;
    doc["confidence"] = score;
    doc["ts"] = millis();
    
    char buffer[200];
    serializeJson(doc, buffer);
    
    return client.publish(CFG_MQTT_TOPIC, buffer);
}
#endif