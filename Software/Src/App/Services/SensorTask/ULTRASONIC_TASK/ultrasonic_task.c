#include "../../../../Cfg.h"
#include "../../../Modules_Driver/MQTT/mqtt.h"
#include "ultrasonic_task.h"
#include "../../../Modules_Driver/ULTRASONIC/ultrasonic.h"
#include "../../../System/RTOS_MANAGER/rtos_manager.h"
#include "../../../System/system_type.h"
#include "esp_log.h"


void Ultrasonic_Task(void *pv)
{
    float distance;
    TickType_t proximityStartTick = 0;
    uint8_t proximityActive = 0;
    uint8_t eventSent = 0;

    ESP_LOGI(TAG_ULTRASONIC,"Ultrasonic Task started");

    while (1)
    {
        Ultrasonic_Main(&distance);

         #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
           ESP_LOGI(TAG_ULTRASONIC, "Distance = %f\n",distance); // Log Trigger pin initialization
         #endif
    
        if (distance > 0)
        {
            /* ---- Proximity Logic ---- */
            if (distance < DOOR_THRESHOLD_CM)
            {
                if (proximityActive == 0)
                {
                    /* First time detect */
                    proximityStartTick = xTaskGetTickCount();
                    proximityActive = 1;
                    eventSent = 0;   // reset
                }
                else
                {
                    /* Check duration */
                    if (!eventSent && (xTaskGetTickCount() - proximityStartTick) >= pdMS_TO_TICKS(DOOR_PROXIMITY_TIME_MS))
                    {
						ESP_LOGI(TAG_ULTRASONIC, "Distance under threshold something in front of the door");
                        xEventGroupSetBits(
                            NotificationEventGroup,
                            EVENT_DOOR_PROXIMITY
                        );  
                        
                        Mqtt_Publish_Int(MQTT_TOPIC_ESP1_ULTRASONIC_ALARM, DOOR_ALARM, 1, 0);

                        eventSent = 1; 
                    }
                }
            }
            else
            {
				ESP_LOGI(TAG_ULTRASONIC, "Distance safe threshold");
                /* Distance back to normal */
                proximityActive = 0;
            }
        }

        Mqtt_Publish_Float(MQTT_TOPIC_ESP1_ULTRASONIC, distance, 1, 0);

        vTaskDelay(pdMS_TO_TICKS(ULTRASONIC_PERIOD_MS));
    }
}


