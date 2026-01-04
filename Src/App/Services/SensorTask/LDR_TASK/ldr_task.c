#include "../../../../Cfg.h"
#include "../../../Modules_Driver/MQTT/mqtt.h"
#include "esp_log.h"

#include "ldr_task.h"
#include "../../../Modules_Driver/LDR/ldr.h"
#include <stdint.h>
#include "../../../System/RTOS_MANAGER/rtos_manager.h"
#include "../../../System/system_type.h"



void Ldr_Task(void *pvParameters)
{
    uint32_t light_percent = 0;

    // uint8_t lowLightActive = 0;   // Flag
    ESP_LOGI(TAG_LDR, "LDR Task started");

    while (1)
    {
        /* Read LDR light intensity */
        light_percent = Ldr_Main();
        Mqtt_Publish_Int(MQTT_TOPIC_ESP1_LDR, light_percent, 1, 0);

        // if (light_percent < LOW_LIGHT_THRESHOLD)
        // {
        //     if (lowLightActive == 0)
        //     {
        //         xEventGroupSetBits(
        //             NotificationEventGroup,
        //             EVENT_LOW_LIGHT
        //         );
        //         lowLightActive = 1;   // Prevent repeat
        //     }
        // }
        // else
        // {
        //     /* Light back to normal */
        //     lowLightActive = 0;
        // }


        vTaskDelay(pdMS_TO_TICKS(LDR_PERIOD_MS));
    }
}



