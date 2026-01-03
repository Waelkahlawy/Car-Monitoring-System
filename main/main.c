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
// Node and Sub-Node IDs - ESP1
#define ESP1_CLIENT_ID              "NOVA_CAR"
#define ESP1_NODE_ID                "ESP1"
#define ESP2_NODE_ID                "ESP2"
#define ESP1_SUB_CABIN_DH11         "DH11"
#define ESP1_SUB_GPS                "GPS"
#define ESP1_SUB_LDR                "LDR"
#define ESP1_SUB_ULTRASONIC         "ULTRASONIC"
#define ESP1_SUB_IMU                "IMU"
#define ESP1_SUB_GSM                "GSM"
#define ESP1_SUB_PREDICTION         "Prediction_Maintenance"
//#define ESP2_SUB_CV_DRIVER          "CV_Driver"

// MQTT Topics Structure: {client_id}/{node_id}/{sub_node_id}/{topic}

#define MQTT_TOPIC_ESP1_CABIN_TEMP          ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_CABIN_DH11 "/temperature"
#define MQTT_TOPIC_ESP1_CABIN_HUMIDITY      ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_CABIN_DH11 "/humidity"
#define MQTT_TOPIC_ESP1_GPS_LATITUDE        ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GPS "/latitude"
#define MQTT_TOPIC_ESP1_GPS_LONGITUDE       ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GPS "/longitude"
#define MQTT_TOPIC_ESP1_GPS_SPEED           ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GPS "/speed"
#define MQTT_TOPIC_ESP1_LDR                 ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_LDR "/light_level"
#define MQTT_TOPIC_ESP1_ULTRASONIC          ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_ULTRASONIC "/distance"
#define MQTT_TOPIC_ESP1_IMU_TEMP            ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_IMU "/temperature_engine"
#define MQTT_TOPIC_ESP1_IMU_RMS             ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_IMU "/rms"
#define MQTT_TOPIC_ESP1_GSM                 ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GSM "/message"
#define MQTT_TOPIC_ESP1_PREDICTION_CONF     ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_PREDICTION "/confidence"
#define MQTT_TOPIC_ESP1_PREDICTION_STATUS   ESP1_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_PREDICTION "/index_status"
//#define MQTT_TOPIC_ESP2_CV_CONFIDENCE       ESP2_CLIENT_ID "/"ESP2_NODE_ID"/"ESP2_SUB_CV_DRIVER "/confidence1"
//#define MQTT_TOPIC_ESP2_CV_STATUS           ESP2_CLIENT_ID"/" ESP2_NODE_ID"/"ESP2_SUB_CV_DRIVER"/index_status1"
// Prediction maintenance messages
static const char* prediction_messages[] = {
    "faulty engine",  // index 0
    "healthy engine"  // index 1
};
        
void app_main(void)
{
   
}
