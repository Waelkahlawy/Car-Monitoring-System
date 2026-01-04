#include "gpio.h"

#if GPIO_ENABLED == STD_ON

#include "esp_log.h"

static const char *g_TAG = TAG_GPIO;

// for status ISR service
static uint8_t g_GpioIsrServiceInstalled = 0;


void Gpio_InitPin(Gpio_ConfigType *pinConfig)
{
    // Configure pull-up/pull-down mode
    gpio_set_pull_mode(pinConfig->pin_num, pinConfig->pin_pull);

    // Set pin direction (input/output/open-drain)
    gpio_set_direction(pinConfig->pin_num, pinConfig->pin_mode);

    // If pin is configured as output, set initial value
    if (pinConfig->pin_mode == GPIO_MODE_OUTPUT ||
        pinConfig->pin_mode == GPIO_MODE_OUTPUT_OD) {
        gpio_set_level(pinConfig->pin_num, pinConfig->pin_value);
    }

/*#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "GPIO %d initialized - Mode: %d, Initial Value: %d, Pull: %d",
             pinConfig->pin_num, pinConfig->pin_mode, pinConfig->pin_value, pinConfig->pin_pull);
#endif
*/
}


uint8_t Gpio_ReadPinValue(gpio_num_t pin_num)
{
    uint8_t value = gpio_get_level(pin_num);

/*#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "GPIO %d read - Value: %d", pin_num, value);
#endif
*/

    return value;
}


void Gpio_WritePinValue(Gpio_ConfigType *pinConfig)
{
    gpio_set_level(pinConfig->pin_num, pinConfig->pin_value);

/*#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "GPIO %d write - Value: %d", pinConfig->pin_num, pinConfig->pin_value);
#endif
*/
}

void Gpio_EnableInterrupt(gpio_num_t pin_num,gpio_int_type_t int_type,Gpio_IsrCallback callback,void *arg)
{
    // Install ISR service once
    if (g_GpioIsrServiceInstalled == 0)
    {
        gpio_install_isr_service(ESP_INTR_FLAG_IRAM); // default flags
        g_GpioIsrServiceInstalled = 1;
    }

    // Set interrupt type (rising, falling, etc.)
    gpio_set_intr_type(pin_num, int_type);

    // Remove any previous ISR handler
    gpio_isr_handler_remove(pin_num);

    // Add ISR handler
    gpio_isr_handler_add(pin_num, callback, arg);

    // Enable interrupt
    gpio_intr_enable(pin_num);

/*
#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "GPIO %d interrupt enabled (type=%d)", pin_num, int_type);
#endif
*/
}

void Gpio_DisableInterrupt(gpio_num_t pin_num)
{
    // Disable interrupt
    gpio_intr_disable(pin_num);

    // Remove ISR handler
    gpio_isr_handler_remove(pin_num);

/*
#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "GPIO %d interrupt disabled", pin_num);
#endif
*/
}


#else
// Stub implementations when GPIO driver is disabled
#warning "GPIO is disabled "

void Gpio_InitPin(Gpio_ConfigType *pinConfig)
{
    // Do nothing
}

uint8_t Gpio_ReadPinValue(gpio_num_t pin_num)
{
        return ESP_ERR_INVALID_STATE;
}

void Gpio_WritePinValue(Gpio_ConfigType *pinConfig)
{
    // Do nothing
}
void Gpio_EnableInterrupt(gpio_num_t pin_num,gpio_int_type_t int_type,Gpio_IsrCallback callback,void *arg)
{

}
void Gpio_DisableInterrupt(gpio_num_t pin_num)
{
    
}

#endif // GPIO_ENABLED