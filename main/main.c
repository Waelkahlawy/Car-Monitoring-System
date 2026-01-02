#include "../Src/Cfg.h"
#include "../Src/Hal/WIFI/wifi.h"
#include "../Src/Hal/GPIO/gpio.h"
#include "../Src/Hal/ADC/adc.h" 
#include "../Src/Hal/I2C/i2c.h"
#include "../Src/Hal/UART/uart.h"
#include "../Src/App/MQTT/mqtt.h"
#include "../Src/App/IMU/imu.h"
#include "../Src/App/LDR/ldr.h"
#include "../Src/App/DHT11/dht11.h"
#include "../Src/App/GPS/gps.h"
#include "../Src/App/GSM/gsm.h"
#include "../Src/App/ULTRASONIC/ultrasonic.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

// Debug TAG
static const char *TAG = TAG_MAIN;

// Node and Sub-Node IDs
#define ESP_CLIENT_ID          "esp32_client"
#define ESP_NODE_ID            "esp_main"
#define GPS_SUB_NODE_ID        "gps"
#define GSM_SUB_NODE_ID        "gsm"
#define DHT11_SUB_NODE_ID      "dht11_cabin"
#define LDR_SUB_NODE_ID        "ldr"
#define ULTRASONIC_SUB_NODE_ID "ultrasonic"
#define IMU_SUB_NODE_ID        "imu"
#define TEMP_ENGINE_SUB_NODE_ID "temp_engine"

// MQTT Topics Structure: {client_id}/{node_id}/{sub_node_id}
#define MQTT_TOPIC_GPS_LOCATION    ESP_CLIENT_ID "/" ESP_NODE_ID "/" GPS_SUB_NODE_ID "/location"
#define MQTT_TOPIC_GPS_SPEED       ESP_CLIENT_ID "/" ESP_NODE_ID "/" GPS_SUB_NODE_ID "/speed"
#define MQTT_TOPIC_GSM             ESP_CLIENT_ID "/" ESP_NODE_ID "/" GSM_SUB_NODE_ID "/message"
#define MQTT_TOPIC_DHT11_CABIN     ESP_CLIENT_ID "/" ESP_NODE_ID "/" DHT11_SUB_NODE_ID "/data"
#define MQTT_TOPIC_LDR             ESP_CLIENT_ID "/" ESP_NODE_ID "/" LDR_SUB_NODE_ID "/headlight"
#define MQTT_TOPIC_ULTRASONIC      ESP_CLIENT_ID "/" ESP_NODE_ID "/" ULTRASONIC_SUB_NODE_ID "/distance"
#define MQTT_TOPIC_IMU_VIBRATION   ESP_CLIENT_ID "/" ESP_NODE_ID "/" IMU_SUB_NODE_ID "/vibration_engine"
#define MQTT_TOPIC_IMU_RMS         ESP_CLIENT_ID "/" ESP_NODE_ID "/" IMU_SUB_NODE_ID "/rms"
#define MQTT_TOPIC_TEMP_ENGINE     ESP_CLIENT_ID "/" ESP_NODE_ID "/" TEMP_ENGINE_SUB_NODE_ID "/temperature"

// LDR threshold for headlight failure detection
#define LDR_THRESHOLD              500
#define IMU_VIBRATION_THRESHOLD    0.1f  // RMS threshold for engine vibration

// Sensor data structures
static Gps_DataType g_Gps_Data;
static Dht11_DataType g_Dht11_Data;
static Imu_DataType g_Imu_Data;
static float g_Ultrasonic_Distance = 0.0f;
static uint32_t g_Ldr_Value = 0;
static float g_Engine_Temp = 0.0f;  // Engine temperature (simulated or from sensor)

// Ultrasonic configuration
static Ultrasonic_Config_t g_Ultrasonic_Config = {
    .trig_pin = TRIGGER_GPIO_PIN,
    .echo_pin = ECHO_GPIO_PIN
};

// Helper function to get current timestamp
static int64_t get_timestamp(void)
{
    return (int64_t)(esp_timer_get_time() / 1000000);  // Convert to seconds
}

// Helper function to create JSON string for GPS location
static void create_gps_location_json(char *buffer, size_t buffer_size, float lat, float lon)
{
    int64_t timestamp = get_timestamp();
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"latitude\":%.6f,"
        "\"longitude\":%.6f,"
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, GPS_SUB_NODE_ID, lat, lon, timestamp);
}

// Helper function to create JSON string for GPS speed
static void create_gps_speed_json(char *buffer, size_t buffer_size, float speed)
{
    int64_t timestamp = get_timestamp();
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"speed_kmh\":%.1f,"
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, GPS_SUB_NODE_ID, speed, timestamp);
}

// Helper function to create JSON string for DHT11 (cabin temperature and humidity)
static void create_dht11_json(char *buffer, size_t buffer_size, float temp, float humidity)
{
    int64_t timestamp = get_timestamp();
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"temperature_c\":%.1f,"
        "\"humidity_percent\":%.1f,"
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, DHT11_SUB_NODE_ID, temp, humidity, timestamp);
}

// Helper function to create JSON string for engine temperature
static void create_temp_engine_json(char *buffer, size_t buffer_size, float temp)
{
    int64_t timestamp = get_timestamp();
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"temperature_c\":%.1f,"
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, TEMP_ENGINE_SUB_NODE_ID, temp, timestamp);
}

// Helper function to create JSON string for LDR (headlight status)
static void create_ldr_json(char *buffer, size_t buffer_size, uint32_t light_value, const char *status)
{
    int64_t timestamp = get_timestamp();
    bool failure = (strcmp(status, "low") == 0);
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"headlight_status\":\"%s\","
        "\"light_value\":%lu,"
        "\"failure_detected\":%s,"
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, LDR_SUB_NODE_ID, status, light_value, 
        failure ? "true" : "false", timestamp);
}

// Helper function to create JSON string for ultrasonic distance
static void create_ultrasonic_json(char *buffer, size_t buffer_size, float distance)
{
    int64_t timestamp = get_timestamp();
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"distance_cm\":%.1f,"
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, ULTRASONIC_SUB_NODE_ID, distance, timestamp);
}

// Helper function to create JSON string for IMU vibration engine
static void create_imu_vibration_json(char *buffer, size_t buffer_size, bool vibration)
{
    int64_t timestamp = get_timestamp();
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"vibration_engine\":%s,"
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, IMU_SUB_NODE_ID, vibration ? "true" : "false", timestamp);
}

// Helper function to create JSON string for IMU RMS value
static void create_imu_rms_json(char *buffer, size_t buffer_size, float rms_value)
{
    int64_t timestamp = get_timestamp();
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"rms_value\":%.3f,"
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, IMU_SUB_NODE_ID, rms_value, timestamp);
}

// Helper function to create JSON string for GSM message
static void create_gsm_json(char *buffer, size_t buffer_size, const char *message_type, 
                            const char *car_status, const char *message)
{
    int64_t timestamp = get_timestamp();
    snprintf(buffer, buffer_size,
        "{"
        "\"client_id\":\"%s\","
        "\"node_id\":\"%s\","
        "\"sub_node_id\":\"%s\","
        "\"message_type\":\"%s\","
        "\"car_status\":\"%s\","
        "\"message\":\"%s\","
        "\"timestamp\":%lld"
        "}",
        ESP_CLIENT_ID, ESP_NODE_ID, GSM_SUB_NODE_ID, message_type, car_status, message, timestamp);
}

// Calculate RMS value from IMU accelerometer data
static float calculate_imu_rms(float accel_x, float accel_y, float accel_z)
{
    return sqrtf((accel_x * accel_x) + (accel_y * accel_y) + (accel_z * accel_z));
}

// Determine if engine vibration is detected based on RMS value
static bool detect_engine_vibration(float rms_value)
{
    return (rms_value > IMU_VIBRATION_THRESHOLD);
}

// Determine LDR headlight status (high/low)
static const char* get_ldr_status(uint32_t light_value)
{
    return (light_value > LDR_THRESHOLD) ? "high" : "low";
}

// Generate GSM message based on sensor readings
static void generate_gsm_message(char *buffer, size_t buffer_size, 
                                 float temp_cabin, float temp_engine, 
                                 bool vibration, const char *ldr_status,
                                 uint32_t iter_count)
{
    const char *message_type = "status";
    const char *car_status = "normal";
    const char *message = "All systems operational";
    
    // Determine car status based on sensor readings
    if (temp_engine > 105.0f) {
        car_status = "critical";
        message_type = "alert";
        message = "Engine temperature critical! Immediate attention required.";
    } else if (temp_engine > 100.0f) {
        car_status = "warning";
        message_type = "warning";
        message = "Engine temperature high. Monitor closely.";
    } else if (temp_cabin > 32.0f) {
        car_status = "warning";
        message_type = "alert";
        message = "Cabin temperature high. Check climate control.";
    } else if (vibration) {
        car_status = "warning";
        message_type = "alert";
        message = "Engine vibration detected. Service recommended.";
    } else if (strcmp(ldr_status, "low") == 0) {
        car_status = "warning";
        message_type = "warning";
        message = "Headlight failure detected. Check lighting system.";
    } else if (iter_count % 10 == 0) {
        car_status = "normal";
        message_type = "info";
        message = "Driver status check: All systems normal.";
    }
    
    create_gsm_json(buffer, buffer_size, message_type, car_status, message);
}

// Environment Task - publishes sensor data to MQTT topics
static void environment_task(void *pvParameters)
{
    char json_buffer[512];
    uint32_t iter_count = 0;
    
    ESP_LOGI(TAG, "Environment task started");
    ESP_LOGI(TAG, "Client ID: %s", ESP_CLIENT_ID);
    ESP_LOGI(TAG, "Node ID: %s", ESP_NODE_ID);
    ESP_LOGI(TAG, "Publishing sensor data every 10 seconds");
    
    while (1) {
        ESP_LOGI(TAG, "=== Publishing Sensor Data - Iteration %lu ===", iter_count);
        
        // 1. Read GPS data
        #if GPS_ENABLED == STD_ON
        if (Gps_Main(&g_Gps_Data) == 0 && g_Gps_Data.valid) {
            // Publish GPS location (separate topic)
            create_gps_location_json(json_buffer, sizeof(json_buffer), 
                                     g_Gps_Data.latitude, g_Gps_Data.longitude);
            Mqtt_Publish(MQTT_TOPIC_GPS_LOCATION, json_buffer, 1, 0);
            ESP_LOGI(TAG, "[GPS] Location: lat=%.6f, lon=%.6f", 
                     g_Gps_Data.latitude, g_Gps_Data.longitude);
            
            // Publish GPS speed (separate topic)
            create_gps_speed_json(json_buffer, sizeof(json_buffer), g_Gps_Data.speed);
            Mqtt_Publish(MQTT_TOPIC_GPS_SPEED, json_buffer, 1, 0);
            ESP_LOGI(TAG, "[GPS] Speed: %.1f km/h", g_Gps_Data.speed);
        }
        #endif
        
        // 2. Read DHT11 (cabin temperature and humidity) - combined in one topic
        #if DHT11_ENABLED == STD_ON
        if (Dht11_Main(&g_Dht11_Data) == 0) {
            float temp_cabin = (float)g_Dht11_Data.temperature;
            float humidity = (float)g_Dht11_Data.humidity;
            create_dht11_json(json_buffer, sizeof(json_buffer), temp_cabin, humidity);
            Mqtt_Publish(MQTT_TOPIC_DHT11_CABIN, json_buffer, 1, 0);
            ESP_LOGI(TAG, "[DHT11] Cabin: temp=%.1f°C, humidity=%.1f%%", temp_cabin, humidity);
        }
        #endif
        
        // 3. Read engine temperature (simulated or from sensor)
        // For now, simulate engine temp as cabin temp + offset
        #if DHT11_ENABLED == STD_ON
        g_Engine_Temp = (float)g_Dht11_Data.temperature + 60.0f;  // Engine is hotter
        #else
        g_Engine_Temp = 85.0f + (iter_count % 20) * 1.5f;  // Simulated
        #endif
        create_temp_engine_json(json_buffer, sizeof(json_buffer), g_Engine_Temp);
        Mqtt_Publish(MQTT_TOPIC_TEMP_ENGINE, json_buffer, 1, 0);
        ESP_LOGI(TAG, "[TEMP_ENGINE] Temperature: %.1f°C", g_Engine_Temp);
        
        // 4. Read LDR (headlight status - high/low)
        #if LDR_ENABLED == STD_ON
        g_Ldr_Value = Ldr_Main();
        const char *ldr_status = get_ldr_status(g_Ldr_Value);
        create_ldr_json(json_buffer, sizeof(json_buffer), g_Ldr_Value, ldr_status);
        Mqtt_Publish(MQTT_TOPIC_LDR, json_buffer, 1, 0);
        ESP_LOGI(TAG, "[LDR] Headlight: %s (value=%lu)", ldr_status, g_Ldr_Value);
        #endif
        
        // 5. Read Ultrasonic distance
        #if ULTRASONIC_ENABLED == STD_ON
        Ultrasonic_Main(&g_Ultrasonic_Distance);
        create_ultrasonic_json(json_buffer, sizeof(json_buffer), g_Ultrasonic_Distance);
        Mqtt_Publish(MQTT_TOPIC_ULTRASONIC, json_buffer, 1, 0);
        ESP_LOGI(TAG, "[ULTRASONIC] Distance: %.1f cm", g_Ultrasonic_Distance);
        #endif
        
        // 6. Read IMU data
        #if IMU_ENABLED == STD_ON
        Imu_Main(&g_Imu_Data);
        
        // Calculate RMS value
        float rms_value = calculate_imu_rms(g_Imu_Data.accel_x, 
                                             g_Imu_Data.accel_y, 
                                             g_Imu_Data.accel_z);
        
        // Detect engine vibration (true/false)
        bool vibration = detect_engine_vibration(rms_value);
        
        // Publish IMU vibration engine status
        create_imu_vibration_json(json_buffer, sizeof(json_buffer), vibration);
        Mqtt_Publish(MQTT_TOPIC_IMU_VIBRATION, json_buffer, 1, 0);
        ESP_LOGI(TAG, "[IMU] Vibration Engine: %s", vibration ? "true" : "false");
        
        // Publish IMU RMS value
        create_imu_rms_json(json_buffer, sizeof(json_buffer), rms_value);
        Mqtt_Publish(MQTT_TOPIC_IMU_RMS, json_buffer, 1, 0);
        ESP_LOGI(TAG, "[IMU] RMS: %.3f", rms_value);
        #endif
        
        // 7. Generate and publish GSM message (JSON format with car status)
        float temp_cabin = 0.0f;
        #if DHT11_ENABLED == STD_ON
        temp_cabin = (float)g_Dht11_Data.temperature;
        #endif
        
        bool vibration_status = false;
        #if IMU_ENABLED == STD_ON
        vibration_status = vibration;
        #endif
        
        const char *ldr_status_str = "high";
        #if LDR_ENABLED == STD_ON
        ldr_status_str = ldr_status;
        #endif
        
        generate_gsm_message(json_buffer, sizeof(json_buffer), 
                           temp_cabin, g_Engine_Temp, 
                           vibration_status, ldr_status_str, iter_count);
        Mqtt_Publish(MQTT_TOPIC_GSM, json_buffer, 1, 0);
        ESP_LOGI(TAG, "[GSM] Message published");
        
        ESP_LOGI(TAG, "✓ All sensor data published successfully in JSON format!");
        
        iter_count++;
        vTaskDelay(pdMS_TO_TICKS(10000));  // Wait 10 seconds
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "NOVA-CAR ESP32 - Multi-Sensor System");
    ESP_LOGI(TAG, "Client ID: %s", ESP_CLIENT_ID);
    ESP_LOGI(TAG, "Node ID: %s", ESP_NODE_ID);
    ESP_LOGI(TAG, "========================================");
    
    // Initialize WiFi
    #if WIFI_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initializing WiFi...");
    Wifi_Init_Sta();
    vTaskDelay(pdMS_TO_TICKS(2000));  // Wait for WiFi connection
    #endif
    
    // Initialize MQTT
    #if MQTT_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initializing MQTT...");
    Mqtt_Init();
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait for MQTT connection
    #endif
    
    // Initialize GPS
    #if GPS_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initializing GPS...");
    Gps_Init();
    #endif
    
    // Initialize DHT11 (cabin sensor)
    #if DHT11_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initializing DHT11 (Cabin)...");
    Dht11_Init();
    #endif
    
    // Initialize LDR
    #if LDR_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initializing LDR...");
    Ldr_Init();
    #endif
    
    // Initialize Ultrasonic
    #if ULTRASONIC_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initializing Ultrasonic...");
    Ultrasonic_Init(&g_Ultrasonic_Config);
    #endif
    
    // Initialize IMU
    #if IMU_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initializing IMU...");
    Imu_Init();
    #endif
    
    // Initialize GSM
    #if GSM_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initializing GSM...");
    GSM_Init();
    #endif
    
    // Create environment task
    ESP_LOGI(TAG, "Creating environment task...");
    xTaskCreate(environment_task, "environment_task", 8192, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "System initialized. Environment task running...");
}
