#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =============================================================
//  CONFIGURATION
// =============================================================
#define STD_ON          1
#define STD_OFF         0
#define ENABLED         STD_ON
#define DISABLED        STD_OFF

// --- Modules ---
#define CFG_MODULE_AI           ENABLED
#define CFG_MODULE_WIFI         ENABLED
#define CFG_MODULE_MQTT         ENABLED
#define CFG_MODULE_WEBSERVER    ENABLED
#define CFG_DEBUG_MODE          ENABLED
#define CFG_MODULE_TELEGRAM     ENABLED

// --- System Settings ---
#define SYS_BAUD_RATE           115200
#define CFG_WIFI_SSID           "DESKTOP-UC4MCI1 1119"
#define CFG_WIFI_PASS           "L279<10g"

// --- Telegram Settings ---
#define CFG_TG_BOT_TOKEN        "8571433290:AAHjToBjkHj84rhQRa3vzYjJmG_gMlQvNS8"
#define CFG_TG_CHAT_ID          "5708590934"
// --- AI Model Source ---
// 1 = From Header (Flash), 0 = From SD Card
#define MODEL_FROM_HEADER       0  
#define CFG_AI_MODEL_FILENAME   "/model.tflite" 
#define CFG_AI_ARENA_SIZE       (1 * 1024 * 1024)
#define CFG_AI_CONF_THRESH      0.30f //0.60f

// --- MQTT & WiFi ---
#define CFG_MQTT_BROKER         "18.197.149.97"   
#define CFG_MQTT_PORT           1883              
#define CFG_MQTT_CLIENT_ID      "ESP32_Driver_Cam1"
#define CFG_MQTT_USER           "asm_user"
#define CFG_MQTT_PASS           "543as"
#define CFG_MQTT_TOPIC_STATUS     "NOVA_CAR/ESP2/CV_Driver/index_status1"
#define CFG_MQTT_TOPIC_CONF       "NOVA_CAR/ESP2/CV_Driver/confidence1"
#define CFG_MQTT_TOPIC_IMAGE    "esp32/cam1/image"

// --- Camera Pins ---

#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM      5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

#endif