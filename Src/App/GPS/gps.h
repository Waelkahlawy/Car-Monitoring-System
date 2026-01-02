
#ifndef GPS_H
#define GPS_H

#include "../../Cfg.h"

#if GPS_ENABLED == STD_ON

#include <stdint.h>
#include <stdbool.h>

// GPS Data Structure
typedef struct {
    float latitude;         // Latitude in decimal degrees
    float longitude;        // Longitude in decimal degrees
    float altitude;         // Altitude in meters
    float speed;            // Speed in km/h
    uint8_t satellites;     // Number of satellites
    bool valid;             // GPS fix valid (true) or not (false)
    uint8_t hour;           // UTC hour
    uint8_t minute;         // UTC minute
    uint8_t second;         // UTC second
} Gps_DataType;

// Function prototypes
void Gps_Init(void);
int Gps_Main(Gps_DataType *data);
void Gps_PowerOn(void);
void Gps_PowerOff(void);

#endif // GPS_ENABLED

#endif // GPS_H