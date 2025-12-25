#include "adc_driver.h"

#if ADC_ENABLED == STD_ON


static const char *TAG = "ADC_DRIVER";


void Adc_Init(Adc_ConfigType *configurations)
{
#if ADC_ENABLED == STD_ON
#if ADC_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "ADC Initialized");
    ESP_LOGI(TAG, "Sensor Channel is: %d", configurations->channel);
    ESP_LOGI(TAG, "Sensor ADC Resolution is: %d", configurations->resolution);
#endif
    
    
    adc1_config_width(configurations->resolution);
    
#if ADC_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "Sensor ADC Attenuation is: %d", configurations->attenuation);
#endif
    
    adc1_config_channel_atten(configurations->channel, configurations->attenuation);
#endif
}

uint32_t Adc_ReadValue(adc_channel_t channel)
{
#if ADC_ENABLED == STD_ON
    
    uint32_t sensorValue = adc1_get_raw(channel);
    
#if ADC_DEBUG_ENABLED == STD_ON
    ESP_LOGI(TAG, "Read sensor value from channel %d and the Value is: %lu", channel, sensorValue);
#endif
    
    return sensorValue;
#else
    return 0;
#endif
}

#endif 