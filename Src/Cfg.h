#ifndef CFG_H
#define CFG_H


//------------------Standard definitions-------------//
#define STD_ON      1 // Enabled
#define STD_OFF     0   // Disabled
<<<<<<< HEAD
<<<<<<< HEAD
 
// Tags for logging
=======

//------------------Tags for Hal logging------------//
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd
=======

//------------------Tags for Hal logging------------//
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd
#define TAG_GPIO        "GPIO_DRIVER"
#define TAG_ADC         "ADC_DRIVER"
#define TAG_I2C         "I2C_DRIVER"
#define TAG_UART        "UART_DRIVER"
#define TAG_WIFI        "WIFI_DRIVER"
#define TAG_IMU         "IMU_DRIVER"
#define TAG_MQTT        "MQTT_DRIVER"
#define TAG_LDR         "LDR_SENSOR"


<<<<<<< HEAD



=======
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd
//------------------Enable or disable Hal drivers------------//
#define GPIO_ENABLED            STD_ON
#define ADC_ENABLED             STD_ON
#define I2C_ENABLED             STD_ON
#define UART_ENABLED            STD_ON
#define WIFI_ENABLED            STD_ON
<<<<<<< HEAD
#define IMU_ENABLED             STD_ON
#define MQTT_ENABLED            STD_ON
#define LDR_ENABLED             STD_ON
=======

//------------------Enable or disable App drivers------------//
#define ULTRASONIC_ENABLED  STD_ON
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd

//------------------Enable or disable App drivers------------//
#define ULTRASONIC_ENABLED  STD_ON

<<<<<<< HEAD
=======

>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd
//------------------Enable or disable debug logs for Hal drivers------------//
// GPIO Debugging
#if GPIO_ENABLED == STD_ON
#define GPIO_DEBUG_ENABLED      STD_ON
#endif
// ADC Debugging
#if ADC_ENABLED == STD_ON
#define ADC_DEBUG_ENABLED       STD_ON
#endif 
// I2C Debugging
#if I2C_ENABLED == STD_ON
#define I2C_DEBUG_ENABLED       STD_ON
#endif
// UART Debugging
#if UART_ENABLED == STD_ON
#define UART_DEBUG_ENABLED      STD_ON
#endif
<<<<<<< HEAD
=======

//------------------Enable or disable debug logs for App drivers------------//
// Ultrasonic Debugging
#if ULTRASONIC_ENABLED == STD_ON
#define ULTRASONIC_DEBUG_ENABLED      STD_ON
#endif

//------------------ Hal driver configration------------//
<<<<<<< HEAD
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd
=======
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd
#if WIFI_ENABLED == STD_ON
#define WIFI_SSID               "WAEL"
#define WIFI_PASSWORD           "Null@987897"
#define WIFI_DEBUG_ENABLED      STD_ON
#endif

<<<<<<< HEAD
<<<<<<< HEAD
#if IMU_ENABLED == STD_ON
#define IMU_DEBUG_ENABLED       STD_ON
// MPU6050 I2C Address
#define MPU6050_ADDR            0x68

// MPU6050 Registers
#define MPU6050_PWR_MGMT_1      0x6B
#define MPU6050_WHO_AM_I        0x75
#define MPU6050_ACCEL_XOUT_H    0x3B
#define MPU6050_GYRO_XOUT_H     0x43
#define MPU6050_TEMP_OUT_H      0x41

#define ACCEL_SENS_2G      16384.0f     // LSB/g
#define GYRO_SENS_250DPS   131.0f       // LSB/(deg/s)

#endif // IMU_ENABLED


// MQTT Configuration & Debugging
#if MQTT_ENABLED == STD_ON
#define MQTT_BROKER_URI         "mqtt://192.168.1.7:1883"
#define MQTT_CLIENT_ID          "esp32_client"
#define MQTT_USERNAME            "WaelKahlawy"
#define MQTT_PASSWORD            "123456789"
#define MQTT_DEBUG_ENABLED      STD_ON
#endif


// LDR Configuration & Debugging
#if LDR_ENABLED == STD_ON
#define LDR_ADC_CHANNEL         ADC_CHANNEL_6       // GPIO34
#define LDR_MIN_VALUE           0                   // Dark (raw ADC)
#define LDR_MAX_VALUE           4095                // Bright (raw ADC)
#define LDR_MIN_LIGHT           0                   // Mapped: Dark = 0%
#define LDR_MAX_LIGHT           100                 // Mapped: Bright = 100%
#define LDR_DEBUG_ENABLED       STD_ON
#endif






















=======
=======
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd
//------------------ App driver configration------------//
//Ultrasonic config
#define ULTRASONIC_SOUND_SPEED_CM_PER_US   (0.034f)
#define ULTRASONIC_DIV_FACTOR              (2.0f)
<<<<<<< HEAD
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd
=======
>>>>>>> cf08a02df6ca5a265fda952ad4a93d10d2438acd



#endif 
