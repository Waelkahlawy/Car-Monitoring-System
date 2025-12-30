
#include "gpio.h"

#if GPIO_ENABLED == STD_ON
#include "esp_log.h"


static const char *g_TAG = TAG_GPIO ;


void Gpio_InitPin(Gpio_ConfigType *pinConfig)
{

    /* Set pin mode (INPUT or OUTPUT) */
    gpio_set_direction(pinConfig->pin_num, pinConfig->pin_mode);

    /* Set initial value if output mode */
    if (pinConfig->pin_mode == GPIO_MODE_OUTPUT) {
        gpio_set_level(pinConfig->pin_num, pinConfig->pin_value);
    }

#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Initialized pin: %d with mode: %d", pinConfig->pin_num, pinConfig->pin_mode);
#endif
}

uint8_t Gpio_ReadPinValue(gpio_num_t pin_num)
{

    uint8_t pin_value = gpio_get_level(pin_num);
    
#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Read pin value: %d from GPIO pin: %d", pin_value, pin_num);
#endif

    return pin_value;

}

void Gpio_WritePinValue(Gpio_ConfigType *pinConfig)
{

    gpio_set_level(pinConfig->pin_num, pinConfig->pin_value);
    
#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Wrote pin value: %d to GPIO pin: %d", pinConfig->pin_value, pinConfig->pin_num);
#endif

}

#endif /* GPIO_ENABLED */