#include "../../../../Cfg.h"
#include "../../../Modules_Driver/MQTT/mqtt.h"
#include "../../../Modules_Driver/IMU/imu.h"
#include "../../../Modules_Driver/GPS/gps.h"

#include "../../Hal/I2C/i2c.h"
#include "../../Hal/UART/uart.h"

#include "sensor_fusion_task.h"
#include "../../../Modules_Driver/SENSOR_FUSION/sensor_fusion.h"
#include "../../../System/RTOS_MANAGER/rtos_manager.h"
#include "../../../System/system_type.h"



#include "esp_log.h"

#if SENSOR_FUSION_ENABLED == STD_ON

static const char *TAG = "FUSION_TASK";

/* Local buffers to store latest sensor data */
static Imu_DataType latest_imu_data = {0};
static Gps_DataType latest_gps_data = {0};


void Sensor_Fusion_Task(void *pv)
{
    Fusion_DataType fused_result = {0};
    
    int return_fun = 0;
    static uint8_t gps_fix_active = 0;
    ESP_LOGI(TAG, "GPS sensor fusion Task started");

    while (1)
    {
        
        Imu_Main( &latest_imu_data , MPU6050_ADDR1);
        return_fun = Gps_Main(&latest_gps_data);

        /* Run sensor fusion algorithm */
        Fusion_Main(&latest_imu_data, &latest_gps_data , &fused_result);
        
        if (return_fun== 0 && latest_gps_data.valid)
        {
                /* Event: GPS Valid Fix */
            if (!gps_fix_active)
                {
                    xEventGroupSetBits(NotificationEventGroup, EVENT_GPS_FIX);
                    gps_fix_active = 1;
                }
        }
        else
        {
            /* Event: GPS Lost */
        if (gps_fix_active)
            {
                xEventGroupSetBits(NotificationEventGroup, EVENT_GPS_LOST);
                gps_fix_active = 0;
            }
        }

        Mqtt_Publish_Float(MQTT_TOPIC_ESP1_GPS_LATITUDE, fused_result.fused_lat, 1, 0);
        Mqtt_Publish_Float(MQTT_TOPIC_ESP1_GPS_LONGITUDE, fused_result.fused_lon, 1, 0);
        Mqtt_Publish_Float(MQTT_TOPIC_ESP1_GPS_SPEED_X, fused_result.velocity_x, 1, 0);
        Mqtt_Publish_Float(MQTT_TOPIC_ESP1_GPS_SPEED_Y, fused_result.velocity_y, 1, 0);
        // Mqtt_Publish(GPS_T_STAMP_TOPIC, fused_result.last_timestamp, 1, 0);

        /* Task delay for fusion update rate */
        vTaskDelay(pdMS_TO_TICKS(FUSION_PERIOD_MS));
    }
}

#endif /* SENSOR_FUSION_ENABLE */