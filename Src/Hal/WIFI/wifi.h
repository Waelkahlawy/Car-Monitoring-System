#ifndef WIFI_H
#define WIFI_H

#include "../../Cfg.h" 

#if WIFI_ENABLED == STD_ON

#include "esp_wifi.h"
#include "esp_event.h"

// Function prototypes
void Wifi_Init_Sta(void); // Initialize Wi-Fi in station mode

#endif // WIFI_ENABLED

#endif // WIFI_H