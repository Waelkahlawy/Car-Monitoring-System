#include <Arduino.h>
#include "src/Config.h"

// --- Include Hardware & Application Modules ---
#include "src/hal/Camera/Camera_HAL.h"
#include "src/hal/Network/WiFi_Manager.h"
#include "src/app/AI/AI_Engine.h"
#include "src/app/Comm/MQTT_Manager.h"
#include "src/app/Web/WebServer.h"
#include "src/app/Telegram_bot/Telegram_Manager.h"
#include "src/mcal/uart/uart_driver.h"
#include "img_converters.h" 

// --- Global Shared Data (Protected by Mutex) ---
SemaphoreHandle_t xDataMutex;
String g_lastLabel = "Init...";
float g_lastScore = 0.0;
int g_lastIndex = 0;

// --- Global Variables for Inter-Task Image Sharing ---
uint8_t* g_alertJpgBuf = NULL; 
size_t g_alertJpgLen = 0;      
bool g_readyToSend = false;    

// --- Task Handles ---
TaskHandle_t hTaskAI = NULL;
TaskHandle_t hTaskComm = NULL;

// --- AI Engine Object ---
#if CFG_MODULE_AI == ENABLED
AIEngine aiEngine;
#endif

// =============================================================
//  TASK 1: AI Processing (Running on Core 1)
// =============================================================
void Task_AI(void *pvParameters) {
    unsigned long lastSnapTime = 0; 
    
    while(1) {
        // 1. Capture Raw Frame
        camera_fb_t* fb = CameraHAL::capture();
        
        if (fb) {
            #if CFG_MODULE_AI == ENABLED
            // 2. Run Inference
            AIResult res = aiEngine.run(fb);
            
            if (res.valid) {
                // 3. Update Shared Data 
                if (xSemaphoreTake(xDataMutex, portMAX_DELAY) == pdTRUE) {
                    g_lastLabel = res.label;
                    g_lastScore = res.score;
                    g_lastIndex = res.index;
                    xSemaphoreGive(xDataMutex);
                }

                // 4. Check for Danger
                if (res.index != 0 && !g_readyToSend && (millis() - lastSnapTime > 5000)) {
                    
                
                    uint8_t* tempBuf = NULL;
                    size_t tempLen = 0;
                    
                
                    bool converted = frame2jpg(fb, 40, &tempBuf, &tempLen);

                    if (converted) {
                        Serial.println("AI: Danger! Data Ready.");
                        g_alertJpgBuf = tempBuf;
                        g_alertJpgLen = tempLen;
                        g_readyToSend = true; 
                        lastSnapTime = millis();
                    }
                }
            }
            #endif
            CameraHAL::release(fb);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// =============================================================
//  TASK 2: Communication (Running on Core 0)
// =============================================================
void Task_Comm(void *pvParameters) {
    #if CFG_MODULE_TELEGRAM == ENABLED
    TelegramManager::init();
    #endif

    #if CFG_MODULE_MQTT == ENABLED
    MQTTManager::init();
    #endif

    vTaskDelay(pdMS_TO_TICKS(2000));

    while(1) {
        // 1. WiFi Auto-Reconnect
        if (!WiFiManager::isConnected()) {
            WiFiManager::connect();
            vTaskDelay(pdMS_TO_TICKS(500));
            continue; 
        }

        // 2. Check if Alert is ready
        if (g_readyToSend && g_alertJpgBuf != NULL) {
            
            // Retrieve Data
            int idx; String lbl; float scr;
            if (xSemaphoreTake(xDataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                idx = g_lastIndex;
                lbl = g_lastLabel;
                scr = g_lastScore;
                xSemaphoreGive(xDataMutex);
            }

            // --- MQTT Section (Data Only) ---
            #if CFG_MODULE_MQTT == ENABLED
            Serial.println("Comm: Sending MQTT Data Only...");
           
            MQTTManager::publishData(lbl.c_str(), scr);
            
            
            // MQTTManager::publishImage(g_alertJpgBuf, g_alertJpgLen); 
            #endif

            // --- Telegram Section (Sends Photo) ---
      
            #if CFG_MODULE_TELEGRAM == ENABLED
            Serial.println("Comm: Sending Telegram...");
            TelegramManager::sendIPG(idx, lbl, scr, g_alertJpgBuf, g_alertJpgLen);
            #endif

            // --- Free Memory ---
            free(g_alertJpgBuf);
            g_alertJpgBuf = NULL;
            g_alertJpgLen = 0;
            g_readyToSend = false; 
            
            Serial.println("Comm: Done.");
        }

        // 3. Regular MQTT Keepalive
        #if CFG_MODULE_MQTT == ENABLED
        MQTTManager::loop();
        #endif

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// =============================================================
//  SETUP FUNCTION
// =============================================================
void setup() {
    Serial.begin(115200);
    UARTDriver::init();
    Serial.println("\n\n--- SYSTEM STARTING ---");

    WiFi.setTxPower(WIFI_POWER_19dBm); 
    xDataMutex = xSemaphoreCreateMutex();

    Serial.println("1. Init Camera...");
    if (!CameraHAL::init()) {
        Serial.println("ERR: Camera Init Failed");
        while(1) delay(100); 
    }
    
    #if CFG_MODULE_AI == ENABLED
    Serial.println("2. Init AI Engine...");
    if (!aiEngine.init()) {
         Serial.println("ERR: AI Init Failed");
    } else {
         Serial.println("AI Engine OK ✅");
    }
    #endif

    Serial.println("3. Connecting WiFi...");
    WiFiManager::connect();
    
    #if CFG_MODULE_WEBSERVER == ENABLED
    WebServer::init();
    #endif

    Serial.println("5. Starting Tasks...");
    xTaskCreatePinnedToCore(Task_AI, "AI_Task", 10 * 1024, NULL, 2, &hTaskAI, 1);
    xTaskCreatePinnedToCore(Task_Comm, "Comm_Task", 8 * 1024, NULL, 1, &hTaskComm, 0);
    
    Serial.println("--- SYSTEM READY ---");
}

void loop() {
    vTaskDelete(NULL);
}