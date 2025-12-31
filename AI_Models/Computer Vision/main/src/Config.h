/**
 *  Config.h
 *  System Configuration using AUTOSAR-style macros
 */
#ifndef CONFIG_H
#define CONFIG_H

// --- Standard Definitions ---
#define STD_ON  1
#define STD_OFF 0

// --- System Configuration ---
#define SYS_USE_PSRAM           STD_ON
#define SYS_SERIAL_DEBUG        STD_ON
#define SYS_BAUD_RATE           115200

// --- Model Source Configuration (SELECT ONE) ---
// Set to STD_ON to load from SD Card, STD_OFF to load from 'model_data.h'
#define CFG_MODEL_LOAD_FROM_SD  STD_ON 

// --- WiFi Credentials ---
#define CFG_WIFI_SSID           "Your Network Name"
#define CFG_WIFI_PASS           "Your Pass"
#define CFG_API_PORT            82

// --- Camera Configuration (AI Thinker) ---
#define CFG_CAM_PIXFORMAT       PIXFORMAT_RGB565
#define CFG_CAM_FRAMESIZE       FRAMESIZE_QVGA
#define CFG_CAM_FB_COUNT        2
#define CFG_CAM_XCLK_FREQ       20000000

// --- AI Engine Configuration ---
#define CFG_AI_ARENA_SIZE       (800 * 1024) // 800KB in PSRAM
#define CFG_AI_CONFIDENCE_THR   0.40f        // 40%
#define CFG_AI_INPUT_WIDTH      128
#define CFG_AI_INPUT_HEIGHT     128
#define CFG_SD_MOUNT_POINT      "/sdcard"
#define CFG_MODEL_FILENAME      "/model.tflite"

// --- Labels ---
static const char* kLabels[] = {
    "safe driving", "texting - right", "talking on the phone - right", 
    "texting - left", "talking on the phone - left", "operating the radio", 
    "Drinking", "reaching behind", "hair and makeup", "talking to passenger"
};
#define CFG_NUM_LABELS          10

#endif // CONFIG_H