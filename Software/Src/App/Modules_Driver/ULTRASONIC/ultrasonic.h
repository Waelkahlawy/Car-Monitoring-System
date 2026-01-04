#ifndef ULTRASONIC_H  
#define ULTRASONIC_H

#include "../../Cfg.h" // Project configuration header

#if ULTRASONIC_ENABLED == STD_ON

#include <stdint.h>
#include "../../Hal/GPIO/gpio.h"
#include "esp_log.h"

/* ==================== Types ==================== */
typedef struct
{
    gpio_num_t trig_pin; // (trigger) pin of the ultrasonic sensor
    gpio_num_t echo_pin; // ECHO pin of the ultrasonic sensor
} Ultrasonic_Config_t; //   ULTRASONIC Configuration Structure


/* ==================== APIs ==================== */
/**
 * Initialize Ultrasonic sensor
 */
void Ultrasonic_Init(Ultrasonic_Config_t *Ultrasonic_Config);
/**
 *  Read distance from ultrasonic sensor
 */
void Ultrasonic_Main(float *Distance);


#endif//ULTRASONIC_ENABLED


#endif /* ULTRASONIC_H */  
