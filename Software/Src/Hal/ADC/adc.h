#ifndef ADC_H
#define ADC_H

#include "../../Cfg.h" // Project configuration header
#include "esp_log.h" // ESP32 logging
#include "driver/adc.h" // ESP32 ADC driver
#include <stdint.h>     // Standard integer types


// ADC Configuration Structure
typedef struct { 
    adc_channel_t channel;    // ADC channel
    adc_bits_width_t resolution; // ADC resolution
    adc_atten_t attenuation;    // ADC attenuation
} Adc_ConfigType;  //   ADC Configuration Structure


void Adc_Init(Adc_ConfigType *configurations); // Initialize ADC with given configurations


uint32_t Adc_ReadValue(adc_channel_t channel); // Read value from specified ADC channel

#endif // ADC_H