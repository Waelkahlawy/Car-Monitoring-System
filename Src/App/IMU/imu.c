#include "imu.h"

#if IMU_ENABLED == STD_ON

#include "../../Hal/I2C/i2c.h"
#include "esp_log.h"
#include <math.h>
// Debug TAG
static const char *g_TAG = TAG_IMU;



void Imu_Init(void)
{

    uint8_t g_who_am_i;
    uint8_t g_reg;

    // Read WHO_AM_I register to verify device
    g_reg = MPU6050_WHO_AM_I;
    I2c_WriteRead(MPU6050_ADDR, &g_reg, 1, &g_who_am_i, 1);
#if IMU_DEBUG_ENABLED == STD_ON
    if (g_who_am_i == 0x68) {
        ESP_LOGI(g_TAG, "MPU6050 detected (WHO_AM_I: 0x%02X)", g_who_am_i);
    } else {
        ESP_LOGE(g_TAG, " MPU6050 not found (WHO_AM_I: 0x%02X)", g_who_am_i);
        return;
    }
#endif

    // Wake up MPU6050 (clear sleep bit in PWR_MGMT_1)
    uint8_t wake_up[] = {MPU6050_PWR_MGMT_1, 0x00};
    I2c_Write(MPU6050_ADDR, wake_up, 2);

#if IMU_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "MPU6050 Initialized");
#endif

}

void Imu_Main(Imu_DataType *data)
{

    uint8_t g_raw_data[14];
    uint8_t g_reg = MPU6050_ACCEL_XOUT_H;

    // Read all sensor data (14 bytes: accel + temp + gyro)
    I2c_WriteRead(MPU6050_ADDR, &g_reg, 1, g_raw_data, 14);
    // Parse accelerometer data
    data->accel_x = (int16_t)((g_raw_data[0] << 8) | g_raw_data[1]);
    data->accel_y = (int16_t)((g_raw_data[2] << 8) | g_raw_data[3]);
    data->accel_z = (int16_t)((g_raw_data[4] << 8) | g_raw_data[5]);
    // Parse temperature data
    data->temperature = (int16_t)((g_raw_data[6] << 8) | g_raw_data[7]);
    // Parse gyroscope data
    data->gyro_x = (int16_t)(((g_raw_data[8] << 8) | g_raw_data[9]));
    data->gyro_y = (int16_t)((g_raw_data[10] << 8) | g_raw_data[11]);
    data->gyro_z = (int16_t)((g_raw_data[12] << 8) | g_raw_data[13]);
        // for calabration
    data->accel_x = ((float)data->accel_x / ACCEL_SENS_2G) - IMU_AX_OFFSET;
    data->accel_y = ((float)data->accel_y / ACCEL_SENS_2G) - IMU_AY_OFFSET;
    data->accel_z = ((float)data->accel_z / ACCEL_SENS_2G) - IMU_AZ_OFFSET;
    
    data->temperature = (data->temperature / 340.0f) + 36.53f;

    data->gyro_x  = ((float)data->gyro_x / GYRO_SENS_250DPS) - IMU_GX_OFFSET;
    data->gyro_y  = ((float)data->gyro_y / GYRO_SENS_250DPS) - IMU_GY_OFFSET;
    data->gyro_z  = ((float)data->gyro_z / GYRO_SENS_250DPS) - IMU_GZ_OFFSET;


#if IMU_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Accel: X=%.2f g, Y=%.2f g, Z=%.2f g | Gyro: X=%.2f °/s, Y=%.2f °/s, Z=%.2f °/s | Temp=%.2f °C",
                 data->accel_x ,
                 data->accel_y ,
                 data->accel_z ,
                 data->gyro_x  ,
                 data->gyro_y  ,
                 data->gyro_z  ,  
                 data->temperature 
    );
          
#endif

}

int Imu_ReadAccel(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z)
{

    uint8_t g_raw_data[6];
    uint8_t g_reg = MPU6050_ACCEL_XOUT_H;

    // Read accelerometer data (6 bytes)
    if (I2c_WriteRead(MPU6050_ADDR, &g_reg, 1, g_raw_data, 6) != 0) {
        return -1;
    }

    // Parse data
    *accel_x = (int16_t)((g_raw_data[0] << 8) | g_raw_data[1]);
    *accel_y = (int16_t)((g_raw_data[2] << 8) | g_raw_data[3]);
    *accel_z = (int16_t)((g_raw_data[4] << 8) | g_raw_data[5]);

#if IMU_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Accel: X=%d, Y=%d, Z=%d", *accel_x, *accel_y, *accel_z);
#endif

    return 0;

}

int Imu_ReadGyro(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z)
{

    uint8_t g_raw_data[6];
    uint8_t g_reg = MPU6050_GYRO_XOUT_H;

    // Read gyroscope data (6 bytes)
    if (I2c_WriteRead(MPU6050_ADDR, &g_reg, 1, g_raw_data, 6) != 0) {
        return -1;
    }

    // Parse data
    *gyro_x = (int16_t)((g_raw_data[0] << 8) | g_raw_data[1]);
    *gyro_y = (int16_t)((g_raw_data[2] << 8) | g_raw_data[3]);
    *gyro_z = (int16_t)((g_raw_data[4] << 8) | g_raw_data[5]);

#if IMU_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Gyro: X=%d, Y=%d, Z=%d", *gyro_x, *gyro_y, *gyro_z);
#endif

    return 0;

}

int Imu_ReadTemp(float *temperature)
{

    uint8_t g_raw_data[2];
    uint8_t g_reg = MPU6050_TEMP_OUT_H;

    // Read temperature data (2 bytes)
    if (I2c_WriteRead(MPU6050_ADDR, &g_reg, 1, g_raw_data, 2) != 0) {
        return -1;
    }

    // Parse and convert to Celsius
    int16_t raw_temp = (int16_t)((g_raw_data[0] << 8) | g_raw_data[1]);
    *temperature = (raw_temp / 340.0) + 36.53;

#if IMU_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Temperature: %.2f °C", *temperature);
#endif

    return 0;

}

float Imu_GetGyroMagnitude(void)
{
    // Read gyroscope data
    int16_t gx, gy, gz;
    if (Imu_ReadGyro(&gx, &gy, &gz) != 0) {
        return 0.0f;
    }

    // Calculate magnitude: sqrt(x² + y² + z²)
    float magnitude = sqrtf((float)(gx * gx + gy * gy + gz * gz));

#if IMU_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Gyro Magnitude: %.2f", magnitude);
#endif

    return magnitude;

}


float Imu_GetAccelMagnitude(void)
{

    // Read accelerometer data
    int16_t ax, ay, az;
    if (Imu_ReadAccel(&ax, &ay, &az) != 0) {
        return 0.0f;
    }

    // Calculate magnitude: sqrt(x² + y² + z²)
    float magnitude = sqrtf((float)(ax * ax + ay * ay + az * az));

#if IMU_DEBUG_ENABLED == STD_ON
    ESP_LOGI(g_TAG, "Accel Magnitude: %.2f", magnitude);
#endif

    return magnitude;

}

#else  

void Imu_Init(void)
{
    // Do nothing
}

void Imu_Main(Imu_DataType *data)
{
    // Do nothing
}

int Imu_ReadAccel(int16_t *accel_x, int16_t *accel_y, int16_t *accel_z)
{
    return -1;
}

int Imu_ReadGyro(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z)
{
    return -1;
}

int Imu_ReadTemp(float *temperature)
{
    return -1;
}

float Imu_GetAccelMagnitude(void)
{
    return 0.0f;
}

float Imu_GetGyroMagnitude(void)
{
    return 0.0f;
}

#endif // IMU_ENABLED