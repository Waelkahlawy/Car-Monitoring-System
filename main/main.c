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

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG_MAIN = "MAIN";

/* ==================== جميع MQTT Topics الخاصة بالمشروع ==================== */
#define MQTT_TOPIC_GPS                      "home/esp32/gps"
#define MQTT_TOPIC_GSM                      "home/esp32/gsm"
#define MQTT_TOPIC_IMU_TRA                  "home/esp32/imu_tracking"
#define MQTT_TOPIC_TEMP_CABIN_ENGINE        "home/esp32/temperature_cabin_engine"
#define MQTT_TOPIC_HUMIDITY                 "home/esp32/humidity"
#define MQTT_TOPIC_ENV                      "home/esp32/environment"
#define MQTT_TOPIC_ULTRASONIC               "home/esp32/ultrasonic"
#define MQTT_TOPIC_RFID                     "home/esp32/rfid"
#define MQTT_TOPIC_LIGHT                    "home/esp32/light"
#define MQTT_TOPIC_IMU_VIB                  "home/esp32/imu_vibration"

void Environment_Task(void *pvParameters)
{
    char payload[256];
    int iter = 0;
    float fake_lat = 30.0444, fake_lon = 31.2357;
    float fake_temp = 25.0, fake_hum = 55.0;
    int fake_sats = 10, fake_signal = -70, fake_distance = 50, fake_light = 500;

    while (1)
    {
        ESP_LOGI(TAG_MAIN, "=== Publishing Dummy Data - Iteration %d ===", iter);

        fake_lat += 0.0001; if (fake_lat > 30.05) fake_lat = 30.0444;
        fake_lon += 0.0001; if (fake_lon > 31.24) fake_lon = 31.2357;
        fake_temp = 22.0 + (iter % 20);
        fake_hum  = 40.0 + (iter % 50);
        fake_sats = 8 + (iter % 6);
        fake_signal = -60 - (iter % 40);
        fake_distance = 20 + (iter % 180);
        fake_light = 100 + (iter % 900);

#if MQTT_ENABLED == STD_ON


        snprintf(payload, sizeof(payload),
                 "{\"latitude\":%.4f,\"longitude\":%.4f,\"satellites\":%d}",
                 fake_lat, fake_lon, fake_sats);
        Mqtt_Publish(MQTT_TOPIC_GPS, payload, 1, 0);

        snprintf(payload, sizeof(payload),
                 "{\"signal_strength\":%d,\"network\":\"Vodafone\"}", fake_signal);
        Mqtt_Publish(MQTT_TOPIC_GSM, payload, 1, 0);

        snprintf(payload, sizeof(payload),
                 "{\"accel_x\":1.23,\"accel_y\":-0.45,\"accel_z\":9.81}");
        Mqtt_Publish(MQTT_TOPIC_IMU_TRA, payload, 1, 0);

        snprintf(payload, sizeof(payload), "%.1f", fake_temp);
        Mqtt_Publish(MQTT_TOPIC_TEMP_CABIN_ENGINE, payload, 1, 0);

        snprintf(payload, sizeof(payload), "%.1f", fake_hum);
        Mqtt_Publish(MQTT_TOPIC_HUMIDITY, payload, 1, 0);

        snprintf(payload, sizeof(payload),
                 "{\"temperature\":%.1f,\"humidity\":%.1f}", fake_temp, fake_hum);
        Mqtt_Publish(MQTT_TOPIC_ENV, payload, 1, 0);

        snprintf(payload, sizeof(payload), "{\"distance_cm\":%d}", fake_distance);
        Mqtt_Publish(MQTT_TOPIC_ULTRASONIC, payload, 1, 0);

        snprintf(payload, sizeof(payload),
                 "{\"card_id\":\"CARD_1234\",\"access_granted\":true}");
        Mqtt_Publish(MQTT_TOPIC_RFID, payload, 1, 0);

        snprintf(payload, sizeof(payload), "%d", fake_light);
        Mqtt_Publish(MQTT_TOPIC_LIGHT, payload, 1, 0);

        snprintf(payload, sizeof(payload),
                 "{\"accel_x\":0.05,\"accel_y\":0.03,\"accel_z\":0.08}");
        Mqtt_Publish(MQTT_TOPIC_IMU_VIB, payload, 1, 0);

        ESP_LOGI(TAG_MAIN, "✓ All 10 dummy topics published successfully!");

#else
        ESP_LOGW(TAG_MAIN, "MQTT disabled — no publish");
#endif

        iter++;
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}


void app_main(void)
{
    ESP_LOGI(TAG_MAIN, "Starting NOVA-CAR ESP32 - Multi-Sensor MQTT Test v2.0");

    Wifi_Init_Sta(); 
       // تشغيل Wi-Fi
    Mqtt_Init();        // تشغيل MQTT

    // إنشاء الـ task لنشر البيانات الوهمية لكل الـ topics
    xTaskCreate(Environment_Task, "multi_dummy_task", 8192, NULL, 5, NULL);

    ESP_LOGI(TAG_MAIN, "Multi-dummy task started - publishing to 10 topics every 10s");
}