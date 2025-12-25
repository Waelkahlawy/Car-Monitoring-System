#ifndef CFG_H
#define CFG_H

#include <stdint.h>

// Standard definitions
#define STD_ON      1 // Enabled
#define STD_OFF     0   // Disabled

#define ADC_ENABLED     STD_ON
#define TAG_GPIO        "GPIO_DRIVER"
#define TAG_ADC         "ADC_DRIVER"


// Enable or disable drivers
#define GPIO_ENABLED            STD_ON
#define ADC_ENABLED             STD_ON

// Enable or disable debug logs for drivers
// GPIO Debugging
#if GPIO_ENABLED == STD_ON
#define GPIO_DEBUG_ENABLED      STD_ON
#endif
// ADC Debugging
#if ADC_ENABLED == STD_ON
#define ADC_DEBUG_ENABLED       STD_ON
#endif 

#endif 
