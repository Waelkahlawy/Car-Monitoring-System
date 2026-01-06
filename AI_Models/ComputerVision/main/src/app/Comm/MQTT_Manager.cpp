#include "MQTT_Manager.h"

#if CFG_MODULE_MQTT == ENABLED

WiFiClient espClient;
PubSubClient client(espClient);

void MQTTManager::init() {
    client.setServer(CFG_MQTT_BROKER, CFG_MQTT_PORT);
  
    client.setBufferSize(5000); 
}

void MQTTManager::loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}

void MQTTManager::reconnect() {
  
    if (WiFi.status() == WL_CONNECTED && !client.connected()) {
        Serial.print("MQTT: Connecting to ");
        Serial.println(CFG_MQTT_BROKER);

        
        if (client.connect(CFG_MQTT_CLIENT_ID, CFG_MQTT_USER, CFG_MQTT_PASS)) {
            Serial.println("MQTT: Connected ✅");
        } else {
            Serial.print("MQTT: Failed, rc=");
            Serial.print(client.state());
            Serial.println(" (Check User/Pass or Firewall)");
            delay(2000); 
        }
    }
}


bool MQTTManager::publishData(const char* label, float score) {
    if (!client.connected()) return false;

    bool sent1 = false;
    bool sent2 = false;

    sent1 = client.publish(CFG_MQTT_TOPIC_STATUS, label);

    String scoreStr = String(score, 1); 
    sent2 = client.publish(CFG_MQTT_TOPIC_CONF, scoreStr.c_str());

    return (sent1 && sent2);
}


bool MQTTManager::publishImage(uint8_t* image_data, size_t image_len) {
    
    if (!client.connected()) {
        Serial.println("MQTT Error: Client disconnected before sending image.");
        return false;
    }
    
    if (image_data == NULL || image_len == 0) return false;

   
    client.loop(); 

   
    size_t requiredSize = image_len + 2048;
    if (!client.setBufferSize(requiredSize)) {
        Serial.println("MQTT Error: Not enough RAM to allocate buffer!");
        return false;
    }

 
    Serial.printf("MQTT: Sending Image (%d bytes)...\n", image_len);
    
    
    bool result = client.publish(CFG_MQTT_TOPIC_IMAGE, image_data, image_len);

   
    client.setBufferSize(512);

    if (result) {
        Serial.println("MQTT: Image Sent 📸");
    } else {
        
        Serial.print("MQTT: Image Failed ❌ - State: ");
        Serial.println(client.state());
        // State -4 = Connection Timeout
        // State -3 = Connection Lost
        // State -1 = Disconnected
    }

    return result;
}

#endif