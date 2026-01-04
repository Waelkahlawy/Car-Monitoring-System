#include "../../../../Cfg.h"
#include "../../../Modules_Driver/MQTT/mqtt.h"
#include "gsm_task.h"
#include "esp_log.h"

#include "../../../Modules_Driver/GSM/gsm.h"
#include "../../../System/RTOS_MANAGER/rtos_manager.h"
#include "../../../System/system_type.h"


void GSM_Event_Task(void *pv)
{
    EventBits_t events;
    SensorMsg_t msg;
    msg.src = SRC_GSM;

    while (1)
    {
        events = xEventGroupWaitBits(
                    NotificationEventGroup,
                    EVENT_DOOR_PROXIMITY |
                    EVENT_ENGINE_VIBRATION |
                    EVENT_OVER_TEMP |
                    // EVENT_LOW_LIGHT |
                    EVENT_HIGH_TEMP |
                    EVENT_HIGH_HUM |
                    EVENT_GPS_LOST,
                    pdTRUE,
                    pdFALSE,
                    portMAX_DELAY
                );

        if (events & EVENT_DOOR_PROXIMITY)
            msg.gsm_alert="ALERT: Object near car door!";

        else if (events & EVENT_ENGINE_VIBRATION)
            msg.gsm_alert="ALERT: Engine vibration!";

        else if (events & EVENT_OVER_TEMP)
            msg.gsm_alert="ALERT: Engine over temperature!";

        // else if (events & EVENT_LOW_LIGHT)
        //     msg.gsm_alert="ALERT: Low light!";

        else if (events & EVENT_HIGH_TEMP)
            msg.gsm_alert="ALERT: Cabin temperature high!";

        else if (events & EVENT_HIGH_HUM)
            msg.gsm_alert="ALERT: Cabin humidity high!";

        else if (events & EVENT_GPS_LOST)
            msg.gsm_alert= "WARNING: GPS lost!";

        /* Send alert to GSM send task */
        xQueueSend(GsmAlertQueue, &msg, 0);
        ESP_LOGI("GSM_SET", "%s",msg.gsm_alert);
    }
}



void GSM_Send_Task(void *pv)
{
    SensorMsg_t msg;

    while (1)
    {
        if (xQueueReceive(GsmAlertQueue, &msg, portMAX_DELAY))
        {
			/* Lock UART */
            xSemaphoreTake(UartMutex, portMAX_DELAY);

            GSM_SendSMS(ALERT_PHONE_NUMBER, msg.gsm_alert);
            
             /* Unlock UART */
            xSemaphoreGive(UartMutex);
            
            ESP_LOGI("GSM_SET", "Send_SMS");
            
            // xQueueSend(SensorQueue, &msg, 0);

        }
    }
}









