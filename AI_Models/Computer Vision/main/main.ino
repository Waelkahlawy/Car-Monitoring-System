#include <Arduino.h>
#include "src/Config.h"
#include "src/Camera_HAL/Camera_HAL.h"
#include "src/AI_Engine/AI_Engine.h"
#include "src/Network_Manager/Network_Manager.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// --- Global Modules ---
AIEngine aiEngine;
String currentLabel = "Waiting...";
float currentScore = 0.0;

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

#if (SYS_SERIAL_DEBUG == STD_ON)
    Serial.begin(SYS_BAUD_RATE);
    Serial.setDebugOutput(true);
    Serial.println("\n--- Driver Monitor System Init ---");
#endif

    if (!CameraHAL::init()) {
        Serial.println("FATAL: Camera Init Failed");
        while(1) delay(1000);
    }
    Serial.println("Camera OK");

    if (!aiEngine.begin()) {
        Serial.println("FATAL: AI Engine Init Failed");
        while(1) delay(1000);
    }
    Serial.println("AI Engine OK");

    // --- التعديل هنا: استخدام الاسم الجديد ---
    AppNetworkManager::connect();
    AppNetworkManager::startServers();

    Serial.println("--- System Ready ---");
}

void loop() {
    // --- التعديل هنا ---
    AppNetworkManager::handleRequests(currentLabel.c_str(), currentScore);

    camera_fb_t* fb = CameraHAL::getFrame();
    if (!fb) {
        // Serial.println("Frame Capture Failed"); // Optional: Comment out to reduce noise
        return;
    }

    CameraHAL::drawBox(fb, 40, 0, 240, 240, 0x07E0); 

    PredictionResult result = aiEngine.runInference(fb);
    
    currentLabel = result.label;
    currentScore = result.score;

    if (result.valid) {
        Serial.printf("Detected: %s (%.1f%%)\n", result.label, result.score * 100);
    }

    CameraHAL::returnFrame(fb);
    
    delay(10);
}