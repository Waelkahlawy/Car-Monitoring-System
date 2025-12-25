#include "gpio_driver.h"  // Include GPIO driver header

#if GPIO_ENABLED == STD_ON // GPIO_ENABLED


static const char *TAG = "GPIO_DRIVER";

==============================================================================================*/

void Gpio_InitPin(Gpio_ConfigType *pinConfig)
{
#if GPIO_ENABLED == STD_ON
    
    gpio_set_direction(pinConfig->pin_num, pinConfig->pin_mode);

    
    if (pinConfig->pin_mode == GPIO_MODE_OUTPUT) {
        gpio_set_level(pinConfig->pin_num, pinConfig->pin_value);
    }

#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "Initialized pin: %d with mode: %d", pinConfig->pin_num, pinConfig->pin_mode);
#endif
#endif
}

uint8_t Gpio_ReadPinValue(gpio_num_t pin_num)
{
#if GPIO_ENABLED == STD_ON
    uint8_t pin_value = gpio_get_level(pin_num);
    
#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "Read pin value: %d from GPIO pin: %d", pin_value, pin_num);
#endif

    return pin_value;
#else
    return 0;
#endif
}

void Gpio_WritePinValue(Gpio_ConfigType *pinConfig)
{
#if GPIO_ENABLED == STD_ON
    gpio_set_level(pinConfig->pin_num, pinConfig->pin_value);
    
#if GPIO_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "Wrote pin value: %d to GPIO pin: %d", pinConfig->pin_value, pinConfig->pin_num);
#endif
#endif
}

#endif /* GPIO_ENABLED */