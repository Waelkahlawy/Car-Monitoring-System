#ifndef PREDICTIVE_MAINTENANCE_H
#define PREDICTIVE_MAINTENANCE_H

#include "../../Cfg.h"


#if PREDICTIVE_MAINTENANCE_ENABLED == STD_ON

#include <stdint.h>


// Feature Structure (3 inputs)
typedef struct {
    float rms_vibration;      // RMS of accel magnitude
    float kurtosis;           // Kurtosis of accel magnitude
    float median_temperature; // Median temperature
} Predictive_FeaturesType;

// Output Structure (binary)
typedef struct {
    uint8_t prediction;       // 0: healthy, 1: faulty
    float confidence;         // Probability score
} Predictive_OutputType;

#ifdef __cplusplus
extern "C" {
#endif

// Function prototypes
void Predictive_Maintenance_Init(void);  // Initialize TFLite interpreter
void Predictive_Maintenance_Run(Predictive_FeaturesType *features, Predictive_OutputType *output);  // Compute features & inference

#ifdef __cplusplus
}
#endif

#endif // PREDICTIVE_MAINTENANCE_ENABLED
#endif // PREDICTIVE_MAINTENANCE_H