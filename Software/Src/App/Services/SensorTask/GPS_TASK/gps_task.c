// #include "../../../../Cfg.h"
// #include "../../../Modules_Driver/MQTT/mqtt.h"
// #include "gps_task.h"
// #include "../../../Modules_Driver/GPS/gps.h"
// #include "../../../System/RTOS_MANAGER/rtos_manager.h"
// #include "../../../System/system_type.h"




// void GPS_Task(void *pv)
// {
//     Gps_DataType data;
//     SensorMsg_t msg;
//     msg.src = SRC_GPS;
   
//     static uint8_t gps_fix_active = 0;
       
//     Gps_PowerOn();

//     while (1)
//     {
// 		/* Lock UART */
//         xSemaphoreTake(UartMutex, portMAX_DELAY);
        
// 		int return_fun = Gps_Main(&data);
		
// 		  /* Unlock UART */
//         xSemaphoreGive(UartMutex);
        
//         if (return_fun== 0 && data.valid)
//         {
//             msg.latitude   = data.latitude;
//             msg.longitude  = data.longitude;
//             msg.satellites = data.satellites;

//             xQueueSend(SensorQueue, &msg, 0);

//             /* Event: GPS Valid Fix */
//           if (!gps_fix_active)
//             {
//                 xEventGroupSetBits(NotificationEventGroup, EVENT_GPS_FIX);
//                 gps_fix_active = 1;
//             }
//         }
//         else
//         {
//             /* Event: GPS Lost */
//            if (gps_fix_active)
//             {
//                 xEventGroupSetBits(NotificationEventGroup, EVENT_GPS_LOST);
//                 gps_fix_active = 0;
//             }
//         }

//         vTaskDelay(pdMS_TO_TICKS(GPS_PERIOD_MS));
//     }
// }




