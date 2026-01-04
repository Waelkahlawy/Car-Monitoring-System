#ifndef SENSOR_FUSION_H
#define SENSOR_FUSION_H

#if SENSOR_FUSION_ENABLED == STD_ON

#include "../IMU/imu.h"
#include "../GPS/gps.h"

// Structure to hold fused results
typedef struct
{
    float fused_lat;
    float fused_lon;
    float velocity_x;
    float velocity_y;
    uint32_t last_timestamp;
}Fusion_DataType;

// Function prototypes
void Fusion_Init(void);
void Fusion_Main(Imu_DataType *imu_data, Gps_DataType *gps_data, Fusion_DataType *fused_result);
#endif

#endif // SENSOR_FUSION_H