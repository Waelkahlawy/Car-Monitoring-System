#ifndef DHT11_H
#define DHT11_H

#include "../../Cfg.h" // Include configuration header

#if DHT11_ENABLED == STD_ON

#include <stdint.h>
#include "esp_err.h"

// Function prototypes
void Dht11_Init(void);                                    // Initialize DHT11 sensor
esp_err_t Dht11_Read(uint8_t* humidity, uint8_t* temperature); // Read data, return ESP_OK on success

#endif // DHT11_ENABLED

#endif // DHT11_H