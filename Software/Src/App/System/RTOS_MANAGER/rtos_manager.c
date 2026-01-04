#include "rtos_manager.h"
#include "../system_type.h"

/* ================= Include Tasks Headers ================= */

#include "../../Services/SensorTask/DHT11_TASK/dht11_task.h"
#include "../../Services/SensorTask/IMU_TASK/imu_task.h"
#include "../../Services/SensorTask/ULTRASONIC_TASK/ultrasonic_task.h"
#include "../../Services/SensorTask/LDR_TASK/ldr_task.h"
#include "../../Services/SensorTask/GPS_TASK/gps_task.h"
#include "../../Services/SensorTask/GSM_TASK/gsm_task.h"
#include "../../Services/AggregateTask/aggregate_task.h"
#include "../../Services/SensorTask/SENSOR_FUSION_TASK/sensor_fusion_task.h"
#include "../../Services/SensorTask/PREDICTIVE_MAINTENANCE_TASK/predictive_maintenance_task.h"


/* ================= Include driver Headers ================= */
#include "../../../Hal/GPIO/gpio.h"
#include "../../../Hal/I2C/i2c.h"
#include "../../../Hal/WIFI/wifi.h"
#include "../../Cfg.h"
#include "../../Modules_Driver/DHT11/dht11.h"
#include "../../Modules_Driver/IMU/imu.h"
#include "../../Modules_Driver/ULTRASONIC/ultrasonic.h"
#include "../../Modules_Driver/LDR/ldr.h"
#include "../../Modules_Driver/GPS/gps.h"
#include "../../Modules_Driver/GSM/gsm.h"
#include "../../Modules_Driver/MQTT/mqtt.h"
#include "../../Modules_Driver/SENSOR_FUSION/sensor_fusion.h"
#include "../../Modules_Driver/PREDICTIVE_MAINTENANCE/predictive_maintenance.h"


/* ================= Queues ================= */
// QueueHandle_t SensorQueue = NULL;
QueueHandle_t GsmAlertQueue = NULL;
/* ================= Mutex ================= */
SemaphoreHandle_t UartMutex = NULL;

/* ================= Event Groups ================= */
EventGroupHandle_t NotificationEventGroup = NULL;

void RTOS_Manager_Init(void)
{
    /* Create Sensor Queue */
    // SensorQueue = xQueueCreate(10, sizeof(SensorMsg_t));
    
    GsmAlertQueue = xQueueCreate(5, sizeof(SensorMsg_t));

    /* Create Event Group */
    NotificationEventGroup = xEventGroupCreate();
        /* Create Uart Mutex*/
    UartMutex = xSemaphoreCreateMutex();
    
    // Ultrasonic 
    
    I2c_ConfigType imu_i2c_conf = {.port =IMU1_I2C_PORT,      
                                   .sda_pin =  IMU_SDA_PIN,        
                                   .scl_pin =  IMU_SCL_PIN,        
                                   .clk_speed =IMU_CLK_SPEED 
                                  };
     
  /*  I2c_ConfigType imu2_i2c_conf = {.port =IMU2_I2C_PORT,      
                                   .sda_pin =  IMU2_SDA_PIN,        
                                   .scl_pin =  IMU2_SCL_PIN,        
                                   .clk_speed =IMU_CLK_SPEED 
                                  };
    */
    Uart_ConfigType gps_uart_conf = {.port =GPS_UART_PORT ,           // UART port number (UART_NUM_0, UART_NUM_1, UART_NUM_2)
                                     .tx_pin =GPS_TX_PIN ,                 // TX pin
                                     .rx_pin =GPS_RX_PIN ,                 // RX pin
                                     .baud_rate =GPS_BAUD_RATE      
                                    };
                                    
    Ultrasonic_Config_t Ultrasonic_Config = {
                                             .trig_pin = TRIGGER_GPIO_PIN,
                                             .echo_pin = ECHO_GPIO_PIN
                                            };
    // init all driver
    I2c_Init(&imu_i2c_conf);
   // I2c_Init(&imu2_i2c_conf);

    Uart_Init(&gps_uart_conf);
    Wifi_Init_Sta(); 
    Mqtt_Init();

    Dht11_Init();
    Gps_Init();
    GSM_Init();
    Imu_Init(MPU6050_ADDR2);
    Imu_Init(MPU6050_ADDR1);
    Ldr_Init();
    Predictive_Maintenance_Init();
    Fusion_Init();
    Ultrasonic_Init(&Ultrasonic_Config);

    //task create
    /* ---------- Create Tasks ---------- */

    /* DHT11 Task */
    xTaskCreate(DHT11_Task,"DHT11_Task",4096,NULL,6,NULL);
    /* IMU Task */
    // xTaskCreate(IMU_Task,"IMU_Task",4096,NULL,3,NULL);
    /* GSM Task */
    xTaskCreate(GSM_Send_Task,"GSM_Send_Task",4096,NULL,7, NULL);
    /* LDR Task */
    xTaskCreate(Ldr_Task,"LDR_Task",4096,NULL,2,NULL);
    /* GPS Task */
    // xTaskCreate(GPS_Task,"GPS_Task",4096,NULL,2,NULL);

    /* Ultrasonic Task */
    xTaskCreate(Ultrasonic_Task,"Ultrasonic_Task",4096,NULL,3,NULL);
    // Aggregate task
    // xTaskCreate(Aggregation_Task,"Aggregation_Task",4096,NULL,2, NULL);

    xTaskCreate(Sensor_Fusion_Task,"Fusion_Task",4096, NULL, 4, NULL);

    xTaskCreate(Predictive_Maintenance_Task,"PredMaint_Task", 8192, NULL, 4, NULL);

}
