#ifndef LDR_H
#define LDR_H

#include "../../Cfg.h" // Include configuration header

#if LDR_ENABLED == STD_ON

#include <stdint.h>     // Include standard integer types

// Function prototypes
void Ldr_Init(void);  // Initialize LDR sensor
uint32_t Ldr_Main(void); // Read LDR value and return light percentage (0-100%)

#endif // LDR_ENABLED

#endif // LDR_H