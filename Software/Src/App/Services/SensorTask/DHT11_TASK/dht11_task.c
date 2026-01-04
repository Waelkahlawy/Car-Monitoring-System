#include "../../../../Cfg.h"
#include "../../../Modules_Driver/MQTT/mqtt.h"
#include "dht11_task.h"
#include "../../../Modules_Driver/DHT11/dht11.h"
#include "../../../System/RTOS_MANAGER/rtos_manager.h"
#include "../../../System/system_type.h"
#include "esp_log.h"



void DHT11_Task(void *pv)
{
    Dht11_DataType data;

    static uint8_t tempHighActive = 0;
    static uint8_t humHighActive  = 0;
    ESP_LOGI(TAG_DHT11, "DHT Task started");

    while (1)
    {
        if (Dht11_Main(&data) == 0)
        {
            /* ---------- Temperature ---------- */
            if (data.temperature > TEMP_HIGH_THRESHOLD)
            {
                if (tempHighActive == 0)
                {
                    xEventGroupSetBits(NotificationEventGroup, EVENT_HIGH_TEMP);
                    tempHighActive = 1;   // latch
                }
            }
            else
            {
                tempHighActive = 0;       // reset when normal
            }

            /* ---------- Humidity ---------- */
            if (data.humidity > HUM_HIGH_THRESHOLD)
            {
                if (humHighActive == 0)
                {
                    xEventGroupSetBits(NotificationEventGroup, EVENT_HIGH_HUM);
                    humHighActive = 1;
                }
            }
            else
            {
                humHighActive = 0;
            }
        }

        Mqtt_Publish_Int(MQTT_TOPIC_ESP1_CABIN_TEMP, data.temperature , 1, 0);

        Mqtt_Publish_Int(MQTT_TOPIC_ESP1_CABIN_HUMIDITY, data.humidity , 1, 0);

        vTaskDelay(pdMS_TO_TICKS(DHT11_PERIOD_MS));
    }
}


