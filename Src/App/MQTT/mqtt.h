#ifndef MQTT_H
#define MQTT_H

#include "../../Cfg.h"

#if MQTT_ENABLED == STD_ON
#include <stdint.h>
#include "mqtt_client.h"

// Callback function type for MQTT data received
typedef void (*Mqtt_DataCallback)(const char *topic, const char *data);

// Function prototypes
void Mqtt_Init(void); // Initialize MQTT client
void Mqtt_Publish(const char *topic, const char *data, int qos, int retain); // Publish message to a topic
void Mqtt_Subscribe(const char *topic, int qos); // Subscribe to a topic
void Mqtt_SetDataCallback(Mqtt_DataCallback callback); // Set user-defined data callback

#endif // MQTT_ENABLED

#endif // MQTT_H