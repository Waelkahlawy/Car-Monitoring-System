// #include "../../../../Cfg.h"
// #include "../../../Modules_Driver/MQTT/mqtt.h"
// #include "imu_task.h"
// #include "../../../Modules_Driver/IMU/imu.h"
// #include <stdint.h>

// #include "../../../System/RTOS_MANAGER/rtos_manager.h"
// #include "../../../System/system_type.h"


// #define ABS(x)   ((x) < 0 ? -(x) : (x))

// void IMU_Task(void *pv)
// {
//     Imu_DataType data;
//     SensorMsg_t msg;
//     msg.src = SRC_IMU;
    
//     static uint8_t vibration_sent = 0;

//     while (1)
//     {
//         Imu_Main(&data);

//         msg.accel_x = data.accel_x;
//         msg.accel_y = data.accel_y;
//         msg.accel_z = data.accel_z;
//         msg.gyro_x =  data.gyro_x;
//         msg.gyro_y =  data.gyro_y;
//         msg.gyro_z =  data.gyro_y;
//         msg.imu_temp = data.temperature;

    
//         xQueueSend(SensorQueue, &msg, 0);

//         // ask for threshold 
//               if (ABS(data.accel_x) > IMU_VIB_THRESHOLD ||
//             ABS(data.accel_y) > IMU_VIB_THRESHOLD ||
//             ABS(data.accel_z) > IMU_VIB_THRESHOLD)
//         {
//             if (!vibration_sent)
//             {
//                 xEventGroupSetBits(NotificationEventGroup, EVENT_ENGINE_VIBRATION);
//                 vibration_sent = 1;
//             }
//         }
//         else
//         {
//             vibration_sent = 0;
//         }

//         vTaskDelay(pdMS_TO_TICKS(IMU_PERIOD_MS));
//     }
// }


