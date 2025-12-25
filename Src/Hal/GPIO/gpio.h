#ifndef GPIO_H   // GPIO_H
#define GPIO__H // GPIO_H

#include "config_file.h" // Include project configuration

#if GPIO_ENABLED == STD_ON // Enable GPIO driver

#include "driver/gpio.h"   // GPIO driver header


typedef struct {
    gpio_num_t pin_num;         // GPIO pin number
    gpio_mode_t pin_mode;       // Pin mode (input/output)
    uint8_t pin_value;          // Pin value (0 or 1)
} Gpio_ConfigType;



void Gpio_InitPin(Gpio_ConfigType *pinConfig); // Initialize GPIO pin


uint8_t Gpio_ReadPinValue(gpio_num_t pin_num); // Read value from GPIO pin


void Gpio_WritePinValue(Gpio_ConfigType *pinConfig); // Write value to GPIO pin

#endif // GPIO_ENABLED

#endif //GPIO_H