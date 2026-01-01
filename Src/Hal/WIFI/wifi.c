#include "wifi.h"

#if WIFI_ENABLED == STD_ON

#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

// Debug TAG
static const char *g_TAG = TAG_WIFI ;

// WiFi event handler
static void Wifi_EventHandler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // Start connecting when Wi-Fi starts
        esp_wifi_connect();
        
#if WIFI_DEBUG_ENABLED == STD_ON
        ESP_LOGI(g_TAG, "Wi-Fi started, connecting...");
#endif
        
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // Auto reconnect on disconnection
        esp_wifi_connect();
        
#if WIFI_DEBUG_ENABLED == STD_ON
        ESP_LOGW(g_TAG, "Wi-Fi disconnected. Reconnecting...");
#endif
        
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // Wi-Fi connected successfully
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        
#if WIFI_DEBUG_ENABLED == STD_ON
        ESP_LOGI(g_TAG, "Wi-Fi connected, IP: " IPSTR, IP2STR(&event->ip_info.ip));
#endif
    }
}

void Wifi_Init_Sta(void)
{
      esp_wifi_set_ps(WIFI_PS_NONE);
    // Initialize NVS (required for Wi-Fi)
    esp_err_t ret = nvs_flash_init(); // Initialize non-volatile storage
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) { // Handle NVS issues
        nvs_flash_erase(); // Erase NVS
        nvs_flash_init(); // Re-initialize NVS
    }

    // Initialize network interface
    esp_netif_init();
    
    // Create default event loop
    esp_event_loop_create_default();
    
    // Create default Wi-Fi station
    esp_netif_create_default_wifi_sta();

    // Default Wi-Fi config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // Register event handlers
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_EventHandler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi_EventHandler, NULL);

    // Configure Wi-Fi credentials
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    // Set Wi-Fi mode to Station (client)
    esp_wifi_set_mode(WIFI_MODE_STA);
    
    // Apply Wi-Fi settings
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    
    // Start Wi-Fi
    esp_wifi_start();
    // Disable power save mode for better performance
  

#if WIFI_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Wi-Fi initialized (SSID: %s)", WIFI_SSID);
#endif
}

#else

// Stub function when WiFi is disabled
void Wifi_Init_Sta(void)
{
    // Do nothing
}

#endif // WIFI_ENABLED