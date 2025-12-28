#ifndef CFG_H
#define CFG_H


//------------------Standard definitions-------------//
#define STD_ON      1 // Enabled
#define STD_OFF     0   // Disabled

//------------------Tags for Hal logging------------//
#define TAG_GPIO        "GPIO_DRIVER"
#define TAG_ADC         "ADC_DRIVER"
#define TAG_I2C         "I2C_DRIVER"
#define TAG_UART        "UART_DRIVER"
#define TAG_WIFI        "WIFI_DRIVER"

//------------------Tags for App logging------------//
#define TAG_ULTRASONIC   "ULTRASONIC_DRIVER"

//------------------Enable or disable Hal drivers------------//
#define GPIO_ENABLED            STD_ON
#define ADC_ENABLED             STD_ON
#define I2C_ENABLED             STD_ON
#define UART_ENABLED            STD_ON
#define WIFI_ENABLED            STD_ON

//------------------Enable or disable App drivers------------//
#define ULTRASONIC_ENABLED  STD_ON


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

//------------------Enable or disable debug logs for App drivers------------//
// Ultrasonic Debugging
#if ULTRASONIC_ENABLED == STD_ON
#define ULTRASONIC_DEBUG_ENABLED      STD_ON
#endif

//------------------ Hal driver configration------------//
//WIFI
#if WIFI_ENABLED == STD_ON
#define WIFI_SSID               "WAEL"
#define WIFI_PASSWORD           "Null987897"
#define WIFI_DEBUG_ENABLED      STD_ON
#endif
//UART
#if UART_ENABLED == STD_ON
#define UART_BAUD_RATE      9600   
#define UART_FRAME_LENGTH   UART_DATA_8_BITS          
#define UART_PARITY_CHECK   UART_PARITY_DISABLE
#define UART_STOP_BIT       UART_STOP_BITS_1
#define UART_FLOW_CTRL      UART_HW_FLOWCTRL_DISABLE
#define UART_SOURCE_CLK      UART_SCLK_DEFAULT
#endif



//------------------ App driver configration------------//
//Ultrasonic config
#if ULTRASONIC_ENABLED == STD_ON
#define ULTRASONIC_SOUND_SPEED_CM_PER_US   (0.034f)
#define ULTRASONIC_DIV_FACTOR              (2.0f)
#endif


#endif 
