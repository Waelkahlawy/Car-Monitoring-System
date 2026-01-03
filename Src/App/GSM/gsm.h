#ifndef GSM_H
#define GSM_H

#include "../../Cfg.h" // Project configuration header

#if GSM_ENABLED == STD_ON
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "../../Hal/UART/uart.h"


typedef enum {
    GSM_OK = 0,
    GSM_ERROR,
    GSM_TIMEOUT,
    GSM_NO_NETWORK
} Gsm_Status_t;

/* Init */
void GSM_Init(void);

/* SMS */
Gsm_Status_t GSM_SendSMS(const char *number, const char *text);

/* Call */
Gsm_Status_t GSM_MakeCall(const char *number);

/* Network */
Gsm_Status_t GSM_GetSignal(uint8_t *rssi);

#endif
#endif





