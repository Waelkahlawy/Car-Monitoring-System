#ifndef DHT11_H
#define DHT11_H

#include "../../Cfg.h" // Include configuration header

#if DHT11_ENABLED == STD_ON

#include <stdint.h>

// DHT11 Data Structure
typedef struct {
    uint8_t temperature;    // Temperature in °C
    uint8_t humidity;       // Humidity in %
} Dht11_DataType;

// Function prototypes
void Dht11_Init(void);
int Dht11_Main(Dht11_DataType *data);

#endif // DHT11_ENABLED

#endif // DHT11_H