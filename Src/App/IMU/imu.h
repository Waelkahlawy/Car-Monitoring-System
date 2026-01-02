
#ifndef IMU_H
#define IMU_H

#include "../../Cfg.h"

#if IMU_ENABLED == STD_ON

#include <stdint.h>



// IMU Data Structure
typedef struct {
    float  accel_x;       // Accelerometer X-axis
    float  accel_y;       // Accelerometer Y-axis
    float  accel_z;       // Accelerometer Z-axis
    float  gyro_x;        // Gyroscope X-axis
    float  gyro_y;        // Gyroscope Y-axis
    float  gyro_z;        // Gyroscope Z-axis
    float  temperature;   // Temperature
} Imu_DataType;

#ifdef __cplusplus
extern "C" {
#endif
// Function prototypes
void Imu_Init(void);                                                        // Initialize IMU sensor                    
void Imu_Main(Imu_DataType *data);                                          // Main IMU data acquisition function
int Imu_ReadAccel(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);    // Read accelerometer data
int Imu_ReadGyro(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);        // Read gyroscope data
int Imu_ReadTemp(float *temperature);                                       // Read temperature data
float Imu_GetGyroMagnitude(void);                                           // Get gyroscope magnitude
float Imu_GetAccelMagnitude(void);                                          // Get accelerometer magnitude                

#ifdef __cplusplus
}
#endif

#endif // IMU_ENABLED
#endif // IMU_H