/* gps.h FILE GAURD */
#ifndef GPS_H_
#define GPS_H_


/* LIBs INCLUSION */
#include <stdio.h>
#include <sys/_intsup.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "hal/ledc_types.h"
#include "portmacro.h"
#include "esp_log.h"
#include "../../Hal/UART/uart.h"


/* GPS LOCATION DATA TYPE */
typedef struct 
{
    float Latitude;
    float Longitude;
    float Altitude;
    float Speed;
    float Course;
    float Timestamp;
}Gps_Parameters_t;



void GPS_Init   (const Uart_ConfigType *UART_Config);
void GPS_GetData(Gps_Parameters_t *GPS_Parametars);




#endif /* GPS_H_ */