
#ifndef IMU_H
#define IMU_H

#include "../../Cfg.h"

#if IMU_ENABLED == STD_ON

#include <stdint.h>



// IMU Data Structure
typedef struct {
    int16_t accel_x;    // Accelerometer X-axis
    int16_t accel_y;    // Accelerometer Y-axis
    int16_t accel_z;    // Accelerometer Z-axis
    int16_t gyro_x;     // Gyroscope X-axis
    int16_t gyro_y;     // Gyroscope Y-axis
    int16_t gyro_z;     // Gyroscope Z-axis
    int16_t temperature; // Temperature
} Imu_DataType;

// Function prototypes
void Imu_Init(void);
void Imu_Main(Imu_DataType *data);
int Imu_ReadAccel(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);
int Imu_ReadGyro(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);
int Imu_ReadTemp(float *temperature);

#endif // IMU_ENABLED

#endif // IMU_H