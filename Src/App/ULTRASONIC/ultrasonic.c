#include "ultrasonic.h" // Include Ultrasonic header

#if ULTRASONIC_ENABLED == STD_ON // Check if Ultrasonic feature is enabled

#include "esp_timer.h"
#include "rom/ets_sys.h"

static const char *g_TAG = "ULTRASONIC_DRIVER"; // Debug TAG

/* Static variables to hold GPIO configuration for TRIG and ECHO pins */
static Gpio_ConfigType g_Trig_Pin;  
static Gpio_ConfigType g_Echo_Pin;  

/* ==================== Ultrasonic Initialization ==================== */
void Ultrasonic_Init(Ultrasonic_Config_t *Ultrasonic_Config)
{
    /* Configure TRIG pin as output */
    g_Trig_Pin.pin_num   = Ultrasonic_Config->trig_pin;  // Assign TRIG GPIO number
    g_Trig_Pin.pin_mode  = GPIO_MODE_OUTPUT;             // Set pin as output
    g_Trig_Pin.pin_value = 0;                            // Initialize output value to LOW
    
    Gpio_InitPin(&g_Trig_Pin);                // Initialize TRIG pin via GPIO driver
   
   // --------------------Debug information----------------------------//
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "Trigger pin Initialized"); // Log Trigger pin initialization
    ESP_LOGI(g_TAG, "Trigger pin is: %d", Ultrasonic_Config->trig_pin); //Trigger pin 
    #endif
   
    /* Configure ECHO pin as input */
    g_Echo_Pin.pin_num   = Ultrasonic_Config->echo_pin;  // Assign ECHO GPIO number
    g_Echo_Pin.pin_mode  = GPIO_MODE_INPUT;             // Set pin as input
    g_Echo_Pin.pin_value = 0;                           // Initialize value (not strictly needed for input)
   
    Gpio_InitPin(&g_Echo_Pin);              // Initialize ECHO pin via GPIO driver
       
     // --------------------Debug information----------------------------//   
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "ECHO pin Initialized"); // Log ECHO pin initialization
    ESP_LOGI(g_TAG, "ECHO pin is: %d", Ultrasonic_Config->echo_pin); //ECHO pin 
    #endif
    /* Ensure TRIG pin is LOW before starting measurement */
    Gpio_WritePinValue(&g_Trig_Pin);  // Write LOW to TRIG pin
    // --------------------Debug information----------------------------//  
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "ULTRASONIC Initialized"); // Log ADC initialization
    #endif
    return;  // Return from initialization function
}

/* ==================== Ultrasonic Measurement ==================== */
void Ultrasonic_Main(float *Distance)
{
    int64_t start_time = 0;  // Variable to store start time of ECHO pulse
    int64_t end_time   = 0;  // Variable to store end time of ECHO pulse

    /* Send 10us trigger pulse */
    g_Trig_Pin.pin_value = 1;            // Set TRIG HIGH
    Gpio_WritePinValue(&g_Trig_Pin);     // Apply value to pin
    ets_delay_us(10);                     // Delay 10 microseconds

    g_Trig_Pin.pin_value = 0;            // Set TRIG LOW
    Gpio_WritePinValue(&g_Trig_Pin);     // Apply value to pin
    // --------------------Debug information----------------------------//   
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "Send Pulse from Trigger pin"); // Log ECHO pin initialization
    #endif
    
    /* Wait for ECHO pin to go HIGH (start of pulse) */
    while (Gpio_ReadPinValue(g_Echo_Pin.pin_num) == 0);  // Busy wait until ECHO HIGH
    start_time = esp_timer_get_time();                   // Record start time in microseconds

    /* Wait for ECHO pin to go LOW (end of pulse) */
    while (Gpio_ReadPinValue(g_Echo_Pin.pin_num) == 1);  // Busy wait until ECHO LOW
    end_time = esp_timer_get_time();                     // Record end time in microseconds

    // --------------------Debug information----------------------------//   
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "Receive Pulse from ECHO pin"); // Log ECHO pin initialization
    #endif
    
    /* Calculate pulse width in microseconds */
    int64_t pulse_width = end_time - start_time;  // Duration of ECHO HIGH pulse
    // --------------------Debug information----------------------------//   
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "Duration of pulse calculated"); // Log ECHO pin initialization
    #endif
    /* Convert pulse width to distance in cm */
    *Distance = (pulse_width * ULTRASONIC_SOUND_SPEED_CM_PER_US) 
                / ULTRASONIC_DIV_FACTOR;  // Apply formula: distance = (time * speed)/2
     // --------------------Debug information----------------------------//  
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "Distance calculated"); // Log ECHO pin initialization
    #endif
    return;  // Return from measurement function
}

#endif 