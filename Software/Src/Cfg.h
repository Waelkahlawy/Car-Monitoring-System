#ifndef CFG_H
#define CFG_H


//------------------Standard definitions-------------//
#define STD_ON      1 // Enabled
#define STD_OFF     0   // Disabled

//------------------Tags for logging -------------//

#define TAG_GPIO                        "GPIO_DRIVER"
#define TAG_ADC                         "ADC_DRIVER"
#define TAG_I2C                         "I2C_DRIVER"
#define TAG_UART                        "UART_DRIVER"
#define TAG_WIFI                        "WIFI_DRIVER"
#define TAG_IMU                         "IMU_DRIVER"
#define TAG_MQTT                        "MQTT_DRIVER"
#define TAG_LDR                         "LDR_SENSOR"
#define TAG_ULTRASONIC                  "ULTRASONIC_DRIVER"
#define TAG_DHT11                       "DHT11"
#define TAG_MAIN                        "MAIN"
#define TAG_GPS                         "GPS_DRIVER"
#define TAG_PREDICTIVE_MAINTENANCE      "GPS_DRIVER"
#define TAG_SENSOR_FUSION               "SENSOR_FUSION"
         
//------------------Enable or disable Hal drivers------------//
#define GPIO_ENABLED                    STD_ON
#define ADC_ENABLED                     STD_ON
#define I2C_ENABLED                     STD_ON
#define UART_ENABLED                    STD_ON
#define WIFI_ENABLED                    STD_ON

//------------------Enable or disable App drivers------------//
#define IMU_ENABLED                     STD_ON
#define MQTT_ENABLED                    STD_ON
#define LDR_ENABLED                     STD_ON
#define ULTRASONIC_ENABLED              STD_ON
#define DHT11_ENABLED                   STD_ON
#define GPS_ENABLED                     STD_ON
#define GSM_ENABLED                     STD_ON
#define PREDICTIVE_MAINTENANCE_ENABLED  STD_ON
#define SENSOR_FUSION_ENABLED           STD_ON

//------------------Configuration & Debugging Options APP Drivers------------//

// MQTT Configuration & Debugging
#if MQTT_ENABLED == STD_ON
#define MQTT_DEBUG_ENABLED              STD_ON
#define MQTT_BROKER_URI                 "mqtt://18.197.149.97:1883"  // Local MQTT broker URI
#define MQTT_CLIENT_ID                  "esp32_client"             // MQTT client ID
#define MQTT_USERNAME                   "asm_user"             // MQTT username
#define MQTT_PASSWORD                   "543as"               // MQTT password

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
#define IMU_DEBUG_ENABLED     STD_OFF
#define IMU1_I2C_PORT         I2C_NUM_0
#define IMU2_I2C_PORT         I2C_NUM_1
#define IMU2_SDA_PIN           GPIO_NUM_12
#define IMU2_SCL_PIN           GPIO_NUM_13
#define IMU_SDA_PIN           GPIO_NUM_21
#define IMU_SCL_PIN           GPIO_NUM_22
#define IMU_CLK_SPEED         400000UL
// MPU6050 I2C Address
#define MPU6050_ADDR1           0x68    
#define MPU6050_ADDR2           0x69    
                                          // Default I2C address
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
#define DHT11_MAX_RETRIES       3                  // Number of read retries.
#define DHT11_TIMEOUT_US     200
#define DHT11_MIN_INTERVAL_US 1000000  // 1 second

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
#define WIFI_SSID               "Wael"           // Your WiFi SSID
#define WIFI_PASSWORD           "12345678"    // Your WiFi Password

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
#define I2C_DEBUG_ENABLED       STD_OFF

#endif

#if ADC_ENABLED == STD_ON
#define ADC_DEBUG_ENABLED       STD_ON  
#endif

#if GPIO_ENABLED == STD_ON
#define GPIO_DEBUG_ENABLED      STD_OFF 
#define HIGH                    1
#define LOW                     0
#endif


/* RTOS dconfigration*/
// DHT
#define DHT11_PERIOD_MS        3000
#define TEMP_HIGH_THRESHOLD   45
#define HUM_HIGH_THRESHOLD    80

// ultrasonic
#define ULTRASONIC_PERIOD_MS      3500
#define DOOR_THRESHOLD_CM         10.0f
#define DOOR_PROXIMITY_TIME_MS    180000         /*180000 // 3 minute*/   
#define DOOR_ALARM                1

//ldr
#define LDR_PERIOD_MS            5000
#define LOW_LIGHT_THRESHOLD      30

//IMU
#define IMU_PERIOD_MS     2000
#define IMU_VIB_THRESHOLD 12000

//gps
#define GPS_PERIOD_MS      5000 

#define FUSION_PERIOD_MS        100     // 10 Hz fusion update (matches IMU)

// engine
#define TEMP_OVER_TEMP_THRESHOLD 200


#define PRED_MAINT_PERIOD_MS     1000U
#define FAULT_CONFIDENCE_THRESHOLD  0.6f   // 70% confidence to trigger alert
#define CONSECUTIVE_FAULTS_REQUIRED 3      // Require 3 consecutive faults to avoid false positives

//mqtt topics

#define ALERT_PHONE_NUMBER  "+201015407032"

#define ESP_CLIENT_ID              "NOVA_CAR"
#define ESP1_NODE_ID               "ESP1"
#define ESP2_NODE_ID               "ESP2"

#define ESP1_SUB_CABIN_DH11         "DH11"
#define ESP1_SUB_GPS                "GPS"
#define ESP1_SUB_LDR                "LDR"
#define ESP1_SUB_ULTRASONIC         "ULTRASONIC"
#define ESP1_SUB_IMU                "IMU"
#define ESP1_SUB_GSM                "GSM"
#define ESP1_SUB_PREDICTION         "Prediction_Maintenance"
#define ESP2_SUB_CV_DRIVER          "CV_Driver"

#define MQTT_TOPIC_ESP1_CABIN_TEMP              ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_CABIN_DH11 "/temperature"
#define MQTT_TOPIC_ESP1_CABIN_HUMIDITY          ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_CABIN_DH11 "/humidity"
#define MQTT_TOPIC_ESP1_GPS_LATITUDE            ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GPS "/latitude"
#define MQTT_TOPIC_ESP1_GPS_LONGITUDE           ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GPS "/longitude"
#define MQTT_TOPIC_ESP1_GPS_SPEED_X             ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GPS "/speed_x"
#define MQTT_TOPIC_ESP1_GPS_SPEED_Y             ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GPS "/speed_y"
#define MQTT_TOPIC_ESP1_LDR                     ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_LDR "/light_level"
#define MQTT_TOPIC_ESP1_ULTRASONIC              ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_ULTRASONIC "/distance"
#define MQTT_TOPIC_ESP1_IMU_TEMP                ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_IMU "/temperature_engine"
#define MQTT_TOPIC_ESP1_IMU_RMS                 ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_IMU "/rms"
#define MQTT_TOPIC_ESP1_GSM                     ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GSM "/message"
#define MQTT_TOPIC_ESP1_PREDICTION_CONF         ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_PREDICTION "/confidence"
#define MQTT_TOPIC_ESP1_PREDICTION_STATUS       ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_PREDICTION "/index_status"

#define MQTT_TOPIC_ESP1_ULTRASONIC_ALARM        ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_ULTRASONIC "/alarm_status"
#define MQTT_TOPIC_ESP1_GPS_T_STAMP             ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_GPS "/time_stamp"
#define MQTT_TOPIC_ESP1_PREDICTION_ENG_ALART    ESP_CLIENT_ID "/" ESP1_NODE_ID "/" ESP1_SUB_PREDICTION "/alart"  


#define  MQTT_TOPIC_ESP2_CV_CONFIDENCE           ESP_CLIENT_ID "/" ESP2_NODE_ID "/" ESP2_SUB_CV_DRIVER "/confidence"
#define  MQTT_TOPIC_ESP2_CV_STATUS               ESP_CLIENT_ID "/" ESP2_NODE_ID "/" ESP2_SUB_CV_DRIVER "/index_status"

#endif // CFG_H
