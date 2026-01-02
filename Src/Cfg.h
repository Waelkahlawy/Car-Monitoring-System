#ifndef CFG_H
#define CFG_H


//------------------Standard definitions-------------//
#define STD_ON      1 // Enabled
#define STD_OFF     0   // Disabled

//------------------Tags for logging -------------//

#define TAG_GPIO        "GPIO_DRIVER"
#define TAG_ADC         "ADC_DRIVER"
#define TAG_I2C         "I2C_DRIVER"
#define TAG_UART        "UART_DRIVER"
#define TAG_WIFI        "WIFI_DRIVER"
#define TAG_IMU         "IMU_DRIVER"
#define TAG_MQTT        "MQTT_DRIVER"
#define TAG_LDR         "LDR_SENSOR"
#define TAG_ULTRASONIC  "ULTRASONIC_DRIVER"
#define TAG_DHT11       "DHT11"
#define TAG_MAIN        "MAIN"
#define TAG_GPS         "GPS_DRIVER"

//------------------Enable or disable Hal drivers------------//
#define GPIO_ENABLED            STD_ON
#define ADC_ENABLED             STD_ON
#define I2C_ENABLED             STD_ON
#define UART_ENABLED            STD_ON
#define WIFI_ENABLED            STD_ON

//------------------Enable or disable App drivers------------//
#define IMU_ENABLED             STD_ON
#define MQTT_ENABLED            STD_ON
#define LDR_ENABLED             STD_ON
#define ULTRASONIC_ENABLED      STD_ON
#define DHT11_ENABLED           STD_ON
#define GPS_ENABLED             STD_ON
#define GSM_ENABLED             STD_ON


//------------------Configuration & Debugging Options APP Drivers------------//

// MQTT Configuration & Debugging
#if MQTT_ENABLED == STD_ON
#define MQTT_DEBUG_ENABLED      STD_ON
#define MQTT_BROKER_URI         "mqtt://192.168.1.3:1883"  // Local MQTT broker URI
#define MQTT_CLIENT_ID          "esp32_client"             // MQTT client ID
#define MQTT_USERNAME            "WaelKahlawy"             // MQTT username
#define MQTT_PASSWORD            "123456789"               // MQTT password

#endif

// LDR Configuration & Debugging
#if LDR_ENABLED == STD_ON
#define LDR_DEBUG_ENABLED       STD_ON   // Enable LDR debug logs
#define LDR_ADC_CHANNEL         ADC_CHANNEL_6       // GPIO34
#define LDR_ADC_WIDTH_BIT       ADC_WIDTH_BIT_12    // 12-bit ADC resolution
#define LDR_ADC_ATTEN_DB        ADC_ATTEN_DB_11     // 11dB attenuation for full-scale voltage
#define LDR_MIN_VALUE           0                   // Dark (raw ADC)
#define LDR_MAX_VALUE           4095                // Bright (raw ADC)
#define LDR_MIN_LIGHT           0                   // Mapped: Dark = 0%
#define LDR_MAX_LIGHT           100                 // Mapped: Bright = 100%

#endif

// Ultrasonic Configuration & Debugging
#if ULTRASONIC_ENABLED == STD_ON
#define ULTRASONIC_DEBUG_ENABLED           STD_ON  // Enable Ultrasonic debug logs
#define TRIGGER_GPIO_PIN                    5
#define ECHO_GPIO_PIN                       18
#define ULTRASONIC_SOUND_SPEED_CM_PER_US   (0.034f)     // Speed of sound in cm/us
#define ULTRASONIC_DIV_FACTOR              (2.0f)   // Division factor for distance calculation
#define ULTRASONIC_TIMEOUT_US              (30000U)
#endif

// IMU Configuration & Debugging
#if IMU_ENABLED == STD_ON
#define IMU_DEBUG_ENABLED      STD_ON
#define IMU_SDA_PIN           GPIO_NUM_21
#define IMU_SCL_PIN           GPIO_NUM_22
// MPU6050 I2C Address
#define MPU6050_ADDR           0x68          // Default I2C address
// MPU6050 Registers
#define MPU6050_PWR_MGMT_1     0x6B         // Power Management register
#define MPU6050_WHO_AM_I       0x75         // Who Am I register
#define MPU6050_ACCEL_XOUT_H   0x3B         // Accelerometer data register
#define MPU6050_GYRO_XOUT_H    0x43         // Gyroscope data register
#define MPU6050_TEMP_OUT_H     0x41         // Temperature data register
#define ACCEL_SENS_2G          16384.0f     // LSB/g
#define GYRO_SENS_250DPS       131.0f       // LSB/(deg/s)

// IMU Calibration Offsets (calculated from static data - X axis facing up, gravity on +X)

// Accelerometer offsets (in g) - subtract these after scaling raw to g
#define IMU_AX_OFFSET    0.03162f
#define IMU_AY_OFFSET   -0.03129f
#define IMU_AZ_OFFSET   -0.07187f
// Gyroscope offsets (in °/s) - subtract these after scaling raw to °/s
#define IMU_GX_OFFSET   -1.72617f
#define IMU_GY_OFFSET    1.13512f
#define IMU_GZ_OFFSET   -0.22767f

#endif // IMU_ENABLED

// DHT11 Sensor Configuration
#if DHT11_ENABLED == STD_ON
#define DHT11_GPIO_PIN          GPIO_NUM_4  
#define DHT11_MAX_RETRIES       3                  // Number of read retries
#define DHT11_DEBUG_ENABLED     STD_ON

#endif



#if GPS_ENABLED == STD_ON
#define GPS_UART_PORT           UART_NUM_2          // UART port for GPS
#define GPS_TX_PIN              GPIO_NUM_17         // TX pin (ESP32 → SIM808 RX)
#define GPS_RX_PIN              GPIO_NUM_16         // RX pin (SIM808 TX → ESP32)
#define GPS_BAUD_RATE           9600                // SIM808 GPS baud rate
#define GPS_BUFFER_SIZE         512                 // GPS data buffer size
#define GPS_DEBUG_ENABLED       STD_ON

#endif

#if GSM_ENABLED == STD_ON
#define GSM_DEBUG_ENABLED       STD_ON
#define GSM_UART                UART_NUM_1
#define GSM_TX_PIN              GPIO_NUM_17
#define GSM_RX_PIN              GPIO_NUM_16
#define GSM_BUF_SIZE            1024

#endif
//------------------Configuration & Debugging Options HAL Drivers------------//

// WIFI Configuration & Debugging
#if WIFI_ENABLED == STD_ON
#define WIFI_DEBUG_ENABLED      STD_ON
#define WIFI_SSID               "WAEL"           // Your WiFi SSID
#define WIFI_PASSWORD           "Null@987897"    // Your WiFi Password

#endif

#if UART_ENABLED == STD_ON
#define UART_DEBUG_ENABLED      STD_ON  
#define UART_BAUD_RATE          9600                        // Baud rate
#define UART_FRAME_LENGTH       UART_DATA_8_BITS            // 8 data bits
#define UART_PARITY_CHECK       UART_PARITY_DISABLE         // No parity
#define UART_STOP_BIT           UART_STOP_BITS_1            // 1 stop bit
#define UART_FLOW_CTRL          UART_HW_FLOWCTRL_DISABLE    // No flow control
#define UART_SOURCE_CLK         UART_SCLK_DEFAULT           // Default source clock

#endif

#if I2C_ENABLED == STD_ON
#define I2C_DEBUG_ENABLED       STD_ON

#endif

#if ADC_ENABLED == STD_ON
#define ADC_DEBUG_ENABLED       STD_ON  
#endif

#if GPIO_ENABLED == STD_ON
#define GPIO_DEBUG_ENABLED      STD_OFF 
#define HIGH                    1
#define LOW                     0
#endif


#endif // CFG_H

