#include "mqtt.h"

#if MQTT_ENABLED == STD_ON

#include "esp_log.h"
#include <string.h>
#include "../WIFI/wifi.h" // Include Wi-Fi header

// Debug TAG
static const char *g_TAG = TAG_MQTT;

// Global MQTT client handle
static esp_mqtt_client_handle_t g_Mqtt_Client = NULL;
// User-defined callback for incoming messages (if needed)
static Mqtt_DataCallback g_User_Callback = NULL;

// MQTT event handler
static void Mqtt_EventHandler(void *handler_args, esp_event_base_t base, 
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
#if MQTT_DEBUG_ENABLED == STD_ON
            ESP_LOGI(g_TAG, "Connected to MQTT broker");
#endif
            break;

        case MQTT_EVENT_DISCONNECTED:
#if MQTT_DEBUG_ENABLED == STD_ON
            ESP_LOGW(g_TAG, "Disconnected from MQTT broker");
#endif
            break;

        case MQTT_EVENT_SUBSCRIBED:
#if MQTT_DEBUG_ENABLED == STD_ON
            ESP_LOGI(g_TAG, "Subscribed to topic, msg_id=%d", event->msg_id);
#endif
            break;

        case MQTT_EVENT_PUBLISHED:
#if MQTT_DEBUG_ENABLED == STD_ON
            ESP_LOGI(g_TAG, "Published message, msg_id=%d", event->msg_id);
#endif
            break;

        case MQTT_EVENT_DATA: {
            char topic[128] = {0};
            char payload[128] = {0};
            
            // Copy topic and payload
            memcpy(topic, event->topic, event->topic_len);
            memcpy(payload, event->data, event->data_len);
            topic[event->topic_len] = '\0';
            payload[event->data_len] = '\0';

#if MQTT_DEBUG_ENABLED == STD_ON
            ESP_LOGI(g_TAG, "Received: %s => %s", topic, payload);
#endif
            // Call user callback if registered
            if (g_User_Callback != NULL) {
                g_User_Callback(topic, payload);
            }
            break;
        
        }
        case MQTT_EVENT_ERROR:
#if MQTT_DEBUG_ENABLED == STD_ON
            ESP_LOGE(g_TAG, "MQTT connection error");
#endif
            break;

        default:
            break;
    }
}

void Mqtt_Init(void)
{

    // MQTT client configuration
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .credentials.client_id = MQTT_CLIENT_ID,
        .credentials.username = MQTT_USERNAME,
        .credentials.authentication.password = MQTT_PASSWORD,  // MQTT password
        .network.disable_auto_reconnect = false,    // Auto reconnect enabled
    };

    // Initialize MQTT client
    g_Mqtt_Client = esp_mqtt_client_init(&mqtt_cfg);
    
    if (!g_Mqtt_Client) {
#if MQTT_DEBUG_ENABLED == STD_ON
        ESP_LOGE(g_TAG, "MQTT init failed");
#endif
        return;
    }

    // Register MQTT event callback
    esp_mqtt_client_register_event(g_Mqtt_Client, ESP_EVENT_ANY_ID, Mqtt_EventHandler, NULL);
    
    // Start MQTT client
    esp_mqtt_client_start(g_Mqtt_Client);

#if MQTT_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "MQTT initialized (Broker: %s, Client ID: %s)", 
             MQTT_BROKER_URI, MQTT_CLIENT_ID);
#endif
}

void Mqtt_Publish(const char *topic, const char *data, int qos, int retain)
{
    if (!g_Mqtt_Client) {
#if MQTT_DEBUG_ENABLED == STD_ON
        ESP_LOGE(g_TAG, "MQTT client not initialized");
#endif
        return;
    }

    esp_mqtt_client_publish(g_Mqtt_Client, topic, data, 0, qos, retain);

#if MQTT_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Published to %s: %s", topic, data);
#endif
}

void Mqtt_Subscribe(const char *topic, int qos)
{
    if (!g_Mqtt_Client) {
#if MQTT_DEBUG_ENABLED == STD_ON
        ESP_LOGE(g_TAG, "MQTT client not initialized");
#endif
        return;
    }

    esp_mqtt_client_subscribe(g_Mqtt_Client, topic, qos);

#if MQTT_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Subscribed to %s", topic);
#endif
}

void Mqtt_SetDataCallback(Mqtt_DataCallback callback)
{
    g_User_Callback = callback;
}

#else 
// Stub functions when MQTT is disabled
void Mqtt_Init(void)
{
    // Do nothing
}

void Mqtt_Publish(const char *topic, const char *data, int qos, int retain)
{
    // Do nothing
}

void Mqtt_Subscribe(const char *topic, int qos)
{
    // Do nothing
}

void Mqtt_SetDataCallback(Mqtt_DataCallback callback)
{
    // Do nothing
}

#endif // MQTT_ENABLED