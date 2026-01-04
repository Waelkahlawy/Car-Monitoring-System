#include "../../../../Cfg.h"
#include "../../../Modules_Driver/MQTT/mqtt.h"

#include "predictive_maintenance_task.h"
#include "../../../Modules_Driver/PREDICTIVE_MAINTENANCE/predictive_maintenance.h"
#include "../../../System/RTOS_MANAGER/rtos_manager.h"
#include "../../../System/system_type.h"

#include "esp_log.h"

#if PREDICTIVE_MAINTENANCE_ENABLED == STD_ON

static const char *TAG = "PRED_MAINT_TASK";

/* Alert thresholds */

void Predictive_Maintenance_Task(void *pv)
{
    Predictive_FeaturesType features = {0};
    Predictive_OutputType output = {0};
    
    uint8_t consecutive_fault_count = 0;
    uint8_t alert_active = 0;
    
    static uint8_t tempOverActive = 0;
    static const char* prediction_messages[] = {
    "faulty engine",  // index 0
    "healthy engine"  // index 1
};
    
    ESP_LOGI(TAG, "Predictive Maintenance Task started");
    
    /* Wait for system stabilization */
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    while (1)
    {
        /* Run inference on collected IMU data */
        Predictive_Maintenance_Run(&features, &output);
        
        /* Check for engine fault prediction */
        if (output.prediction == 1 && output.confidence >= FAULT_CONFIDENCE_THRESHOLD)
        {
            consecutive_fault_count++;
            
            ESP_LOGW(TAG, "Fault detected! Count: %d/%d, Confidence: %.2f%%", consecutive_fault_count, CONSECUTIVE_FAULTS_REQUIRED, output.confidence * 100.0f);
            
            /* Trigger alert only after consecutive faults */
            if (consecutive_fault_count >= CONSECUTIVE_FAULTS_REQUIRED && !alert_active)
            {
                /* Set event group bit for notification */
                Mqtt_Publish_Int(MQTT_TOPIC_ESP1_PREDICTION_ENG_ALART, output.prediction, 1, 0);
                alert_active = 1;
            }
        }
        else
        {
            /* Reset consecutive fault counter if healthy */
            if (consecutive_fault_count > 0)
            {
                ESP_LOGI(TAG, "Engine healthy - resetting fault counter");
            }
            consecutive_fault_count = 0;
            alert_active = 0;
        }
    
        if (features.median_temperature > TEMP_OVER_TEMP_THRESHOLD)
        {
            if (tempOverActive == 0)
            {
                Mqtt_Publish_Float(MQTT_TOPIC_ESP1_PREDICTION_ENG_ALART, features.median_temperature, 1, 0);
                xEventGroupSetBits(NotificationEventGroup, EVENT_OVER_TEMP);
                tempOverActive = 1;   // latch
            }
        }
        else
        {
            tempOverActive = 0;       // reset when normal
        }


        ESP_LOGD(TAG, "Status sent: %s (Conf: %.2f%%, RMS: %.3f, Kurt: %.3f, Temp: %.2f°C)",
                    output.prediction ? "FAULTY" : "HEALTHY",
                    output.confidence * 100.0f,
                    features.rms_vibration,
                    features.kurtosis,
                    features.median_temperature);

        Mqtt_Publish(MQTT_TOPIC_ESP1_PREDICTION_STATUS,prediction_messages[output.prediction], 1, 0);
        Mqtt_Publish_Float(MQTT_TOPIC_ESP1_PREDICTION_CONF, output.confidence, 1, 0);
        Mqtt_Publish_Float(MQTT_TOPIC_ESP1_IMU_RMS, features.rms_vibration, 1, 0);
        Mqtt_Publish_Float(MQTT_TOPIC_ESP1_IMU_TEMP, features.median_temperature, 1, 0);

        /* Task delay for inference period */
        vTaskDelay(pdMS_TO_TICKS(PRED_MAINT_PERIOD_MS));
    }
}

#endif /* PREDICTIVE_MAINTENANCE_ENABLED */