#include "adc.h" // Include ADC header

#if ADC_ENABLED == STD_ON // Check if ADC feature is enabled


static const char *g_TAG = "ADC_DRIVER"; // Debug TAG


void Adc_Init(Adc_ConfigType *configurations) // Initialize ADC with given configurations
{

#if ADC_DEBUG_ENABLED == STD_ON // Debug information
    ESP_LOGI(g_TAG, "ADC Initialized"); // Log ADC initialization
    ESP_LOGI(g_TAG, "Sensor Channel is: %d", configurations->channel); // Log ADC channel
    ESP_LOGI(g_TAG, "Sensor ADC Resolution is: %d", configurations->resolution); // Log ADC resolution
#endif
    
    
    adc1_config_width(configurations->resolution); // Configure ADC resolution
    
#if ADC_DEBUG_ENABLED == STD_ON // Debug information
    ESP_LOGI(g_TAG, "Sensor ADC Attenuation is: %d", configurations->attenuation); // Log ADC attenuation
#endif
    
    adc1_config_channel_atten(configurations->channel, configurations->attenuation); // Configure ADC channel attenuation

}

// Read value from specified ADC channel
uint32_t Adc_ReadValue(adc_channel_t channel) 
{

    
    uint32_t sensorValue = adc1_get_raw(channel); // Read raw ADC value from specified channel
    
#if ADC_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Read sensor value from channel %d and the Value is: %lu", channel, sensorValue); // Log read sensor value

#endif
return sensorValue; // Return the read sensor value

}
#else 
#warning "ADC is disabled — functions will return error."

void Adc_Init(Adc_ConfigType *configurations)
{
    // Do Nothing
}
uint32_t Adc_ReadValue(adc_channel_t channel) 
{
    return ESP_ERR_INVALID_STATE;
}

#endif 