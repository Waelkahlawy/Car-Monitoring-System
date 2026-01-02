#include <Arduino.h>
#include "src/Config.h"

// Include Modules
#include "src/hal/Camera/Camera_HAL.h"
#include "src/hal/Network/WiFi_Manager.h"
#include "src/app/AI/AI_Engine.h"
#include "src/app/Comm/MQTT_Manager.h"
#include "src/app/Web/WebServer.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// --- Global Shared Data ---
SemaphoreHandle_t xDataMutex;
String g_lastLabel = "Init...";
float g_lastScore = 0.0;

// --- Task Handles ---
TaskHandle_t hTaskAI = NULL;
TaskHandle_t hTaskComm = NULL;

// --- Global Objects ---
#if CFG_MODULE_AI == ENABLED
AIEngine aiEngine;
#endif

// =============================================================
//  TASK 1: AI Processing
// =============================================================
void Task_AI(void *pvParameters) {
    while(1) {
        camera_fb_t* fb = CameraHAL::capture();
        if (fb) {
            #if CFG_MODULE_AI == ENABLED
            AIResult res = aiEngine.run(fb);
            if (res.valid) {
                xSemaphoreTake(xDataMutex, portMAX_DELAY);
                g_lastLabel = res.label;
                g_lastScore = res.score;
                xSemaphoreGive(xDataMutex);
                
                #if CFG_DEBUG_MODE
                
                Serial.printf("AI Detect: %s (%.1f%%)\n", res.label, res.score);
                #endif
            }
            #endif
            CameraHAL::release(fb);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// =============================================================
//  TASK 2: Communication (WiFi & MQTT Logging)
// =============================================================
void Task_Comm(void *pvParameters) {
    #if CFG_MODULE_MQTT == ENABLED
    MQTTManager::init();
    #endif

    
    vTaskDelay(pdMS_TO_TICKS(2000));

    while(1) {
        
        if (!WiFiManager::isConnected()) {
            Serial.println("WiFi: Connecting...");
            WiFiManager::connect();
            
            int attempts = 0;
            while (!WiFiManager::isConnected() && attempts < 20) {
                vTaskDelay(pdMS_TO_TICKS(500));
                attempts++;
                if (attempts % 2 == 0) Serial.print("."); 
            }
            Serial.println("");

            if (WiFiManager::isConnected()) {
                Serial.print("WiFi Connected! IP: ");
                Serial.println(WiFi.localIP());
            } else {
                Serial.println("WiFi Failed. Retry in 5s...");
                vTaskDelay(pdMS_TO_TICKS(5000)); 
                continue; 
            }
        }

        
        #if CFG_MODULE_MQTT == ENABLED
        MQTTManager::loop();
        
        String label;
        float score;
        
        if (xSemaphoreTake(xDataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            label = g_lastLabel;
            score = g_lastScore;
            xSemaphoreGive(xDataMutex);
            
            if (WiFiManager::isConnected()) {
                
                Serial.print("MQTT -> [");
                Serial.print(label);
                Serial.print(" : ");
                Serial.print(score, 1);
                Serial.print("%] ... ");

                bool sent = MQTTManager::publish(label.c_str(), score);
                
                if (sent) Serial.println("OK ✅");
                else      Serial.println("FAIL ❌");
            }
        }
        #endif

        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

// =============================================================
//  SETUP
// =============================================================
void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
    Serial.begin(SYS_BAUD_RATE);
    Serial.println("\n\n--- SYSTEM STARTING ---");

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
        Serial.println("AI Engine OK");
    }
    #endif

  
    Serial.println("3. Connecting WiFi...");
    WiFiManager::connect();

   
    #if CFG_MODULE_WEBSERVER == ENABLED
    Serial.println("4. Starting Web Server...");
    WebServer::init();
    #endif

   
    Serial.println("5. Starting Tasks...");
    xTaskCreatePinnedToCore(Task_AI, "AI_Task", 10 * 1024, NULL, 2, &hTaskAI, 1);
    xTaskCreatePinnedToCore(Task_Comm, "Comm_Task", 4 * 1024, NULL, 1, &hTaskComm, 0);

    Serial.println("--- SYSTEM READY ---");
}

void loop() {
    vTaskDelete(NULL);
}