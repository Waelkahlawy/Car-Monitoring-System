#include "ultrasonic.h" // Include Ultrasonic header

#if ULTRASONIC_ENABLED == STD_ON // Check if Ultrasonic feature is enabled

#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "esp_attr.h"


static const char *g_TAG = TAG_ULTRASONIC ; // Debug TAG

/* Static variables to hold GPIO configuration for TRIG and ECHO pins */
static Gpio_ConfigType g_Trig_Pin;  
static Gpio_ConfigType g_Echo_Pin;  

/* Static variables to hold Time and flag for receive ECHO*/
static volatile int64_t g_echo_start_time = 0;
static volatile int64_t g_echo_end_time   = 0;
static volatile uint8_t g_echo_done_flag  = 0;

/* function to initialize ECHO PIN as Interrupt*/
void Ultrasonic_EchoInterruptInit(void);

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
   
    Gpio_InitPin(&g_Echo_Pin);              // Initialize ECHO pin via GPIO driver
     // --------------------Debug information----------------------------//   
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "ECHO pin Initialized"); // Log ECHO pin initialization
    ESP_LOGI(g_TAG, "ECHO pin is: %d", Ultrasonic_Config->echo_pin); //ECHO pin 
    #endif
    
    /* Ensure TRIG pin is LOW before starting measurement */
    Gpio_WritePinValue(&g_Trig_Pin);  // Write LOW to TRIG pin
    // inti ECHO as interrupt pin
    Ultrasonic_EchoInterruptInit();
    
    // --------------------Debug information----------------------------//  
    #if ULTRASONIC_DEBUG_ENABLED == STD_ON 
    ESP_LOGI(g_TAG, "ULTRASONIC Initialized"); // Log ADC initialization
    #endif
    return;  // Return from initialization function
}


void Ultrasonic_Main(float *Distance)
{
    g_echo_done_flag = 0;

    /* Trigger pulse */
    g_Trig_Pin.pin_value = 0;
    Gpio_WritePinValue(&g_Trig_Pin);
    ets_delay_us(2);

    g_Trig_Pin.pin_value = 1;
    Gpio_WritePinValue(&g_Trig_Pin);
    ets_delay_us(10);

    g_Trig_Pin.pin_value = 0;
    Gpio_WritePinValue(&g_Trig_Pin);
    

    /* Wait for measurement to complete */
    int64_t timeout_start = esp_timer_get_time();

    while (!g_echo_done_flag)
    {
        if ((esp_timer_get_time() - timeout_start) > ULTRASONIC_TIMEOUT_US)
        {
            *Distance = -1.0f;
            return;
        }
    }

    /*calculate Pulse width time*/
    int64_t pulse_width = g_echo_end_time - g_echo_start_time;
    /*calculate distance*/
    *Distance = (pulse_width * ULTRASONIC_SOUND_SPEED_CM_PER_US)
                / ULTRASONIC_DIV_FACTOR;
}

 // ISR for ECHO pin, placed in IRAM for interrupt safety
static void IRAM_ATTR Ultrasonic_Echo_ISR(void *arg)
{
    int level = gpio_get_level(g_Echo_Pin.pin_num);  // Read current logic level of ECHO pin (HIGH or LOW)
    int64_t now = esp_timer_get_time();     // Get current timestamp in microseconds
    
    if (level == 1)   // Check if this interrupt is a rising edge
    {
        g_echo_start_time = now; // Save start time of ECHO pulse
    }
    else   // Otherwise, it is a falling edge
    {
        g_echo_end_time = now;  // Save end time of ECHO pulse
        g_echo_done_flag = 1;  // Indicate that pulse measurement is complete
    }
}


/* ==================== Ultrasonic ECHO Interrupt Init ==================== */
void Ultrasonic_EchoInterruptInit(void)
{
	// Initialize ECHO GPIO interrupt
    gpio_config_t echo_int_conf = {
        .pin_bit_mask = (1ULL << g_Echo_Pin.pin_num),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_ANYEDGE   // Enable interrupt on rising and falling edges
    };

    gpio_config(&echo_int_conf);

    /* Install ISR service */
    static uint8_t isr_service_installed = 0;
    if (!isr_service_installed)
    {
        gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
        isr_service_installed = 1;
    }

    /* Attach ISR handler */
    gpio_isr_handler_add(
        g_Echo_Pin.pin_num,
        Ultrasonic_Echo_ISR,
        NULL
    );

#if ULTRASONIC_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "ECHO Interrupt Initialized on GPIO %d", g_Echo_Pin.pin_num);
#endif
}


#endif 