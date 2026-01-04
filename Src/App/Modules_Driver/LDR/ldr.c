#include "ldr.h"

#if LDR_ENABLED == STD_ON

#include "../../Hal/ADC/adc.h"
#include "esp_log.h"

// Debug TAG
static const char *g_TAG = TAG_LDR;

// Global ADC configuration for LDR
static Adc_ConfigType g_Ldr_AdcConfig;

void Ldr_Init(void)
{
    // Configure ADC for LDR
    g_Ldr_AdcConfig.channel = LDR_ADC_CHANNEL; // ADC channel for LDR
    g_Ldr_AdcConfig.resolution = LDR_ADC_WIDTH_BIT; //  12-bit resolution
    g_Ldr_AdcConfig.attenuation = LDR_ADC_ATTEN_DB; //   11dB attenuation for full-scale voltage

    // Initialize ADC
    Adc_Init(&g_Ldr_AdcConfig);

#if LDR_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "LDR Sensor Initialized on channel %d", LDR_ADC_CHANNEL);
#endif
}

uint32_t Ldr_Main(void)
{
    // Read raw ADC value
    uint32_t raw_value = Adc_ReadValue(LDR_ADC_CHANNEL);

    // Map raw value (0-4095) to light percentage (0-100%)
    uint32_t light_percentage = (raw_value * (LDR_MAX_LIGHT - LDR_MIN_LIGHT)) / (LDR_MAX_VALUE - LDR_MIN_VALUE);

    // Constrain to 0-100%
    if (light_percentage > 100) {
        light_percentage = 100;
    }

#if LDR_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "LDR Raw: %lu, Light: %lu%%", raw_value, light_percentage);
#endif

    return light_percentage;
}

#else

// Stub functions when LDR is disabled
void Ldr_Init(void)
{
    // Do nothing
}

uint32_t Ldr_Main(void)
{
    return ESP_ERR_INVALID_STATE;
}   

#endif // LDR_ENABLED