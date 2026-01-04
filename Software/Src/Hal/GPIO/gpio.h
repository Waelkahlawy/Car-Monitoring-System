#ifndef GPIO_H
#define GPIO_H

#include "../../Cfg.h"


#include "driver/gpio.h"
#include <stdint.h>


// GPIO Configuration Structure
typedef struct {
    gpio_num_t pin_num;         // GPIO pin number (e.g., GPIO_NUM_4)
    gpio_mode_t pin_mode;       // Pin mode: GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_OUTPUT_OD, etc.
    uint8_t pin_value;          // Initial output value (0 or 1), used only in output mode
    gpio_pull_mode_t pin_pull;   // Pull mode: GPIO_PULLUP_ONLY, GPIO_PULLDOWN_ONLY, GPIO_FLOATING, GPIO_PULLUP_PULLDOWN
} Gpio_ConfigType;

// pointer for call back function
typedef void (*Gpio_IsrCallback)(void *arg);


// Function prototypes
void Gpio_InitPin(Gpio_ConfigType *pinConfig);       // Initialize a GPIO pin with full configuration
uint8_t Gpio_ReadPinValue(gpio_num_t pin_num);       // Read current value from a GPIO pin
void Gpio_WritePinValue(Gpio_ConfigType *pinConfig); // Write value to a GPIO pin (output mode)


// Interrupt APIs
void Gpio_EnableInterrupt(gpio_num_t pin_num,gpio_int_type_t int_type,Gpio_IsrCallback callback,void *arg);
void Gpio_DisableInterrupt(gpio_num_t pin_num);



#endif // GPIO_H