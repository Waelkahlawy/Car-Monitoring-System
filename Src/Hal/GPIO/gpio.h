#ifndef GPIO_H
#define GPIO_H

#include "../../Cfg.h"

#if GPIO_ENABLED == STD_ON

#include "driver/gpio.h"
#include <stdint.h>

// GPIO Configuration Structure
typedef struct {
    gpio_num_t pin_num;         // GPIO pin number (e.g., GPIO_NUM_4)
    gpio_mode_t pin_mode;       // Pin mode: GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_OUTPUT_OD, etc.
    uint8_t pin_value;          // Initial output value (0 or 1), used only in output mode
} Gpio_ConfigType;

// Function prototypes
void Gpio_InitPin(Gpio_ConfigType *pinConfig);       // Initialize a GPIO pin with full configuration
uint8_t Gpio_ReadPinValue(gpio_num_t pin_num);       // Read current value from a GPIO pin
void Gpio_WritePinValue(Gpio_ConfigType *pinConfig); // Write value to a GPIO pin (output mode)

#endif // GPIO_ENABLED

#endif // GPIO_H