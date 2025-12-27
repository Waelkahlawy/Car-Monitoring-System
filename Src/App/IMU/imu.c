#include "imu.h"

#if IMU_ENABLED == STD_ON

#include "../../Hal/I2C/i2c.h"
#include "esp_log.h"

// Debug TAG
static const char *g_TAG = TAG_IMU;

void send_teleplot_data(Imu_DataType *data) {
    // Method A: Simple printf (most reliable)
    printf(">accel_x:%d\n", data->accel_x);
    printf(">accel_y:%d\n", data->accel_y);
    printf(">accel_z:%d\n", data->accel_z);
    printf(">gyro_x:%d\n", data->gyro_x);
    printf(">gyro_y:%d\n", data->gyro_y);
    printf(">gyro_z:%d\n", data->gyro_z);
    printf(">temp:%d\n", data->temperature);
    
    // Method B: With timestamp
    // int64_t time_ms = esp_timer_get_time() / 1000;
    // printf("@%lld>accel_x:%.3f\n", time_ms, data->accel_x);
    
    fflush(stdout); // Ensure data is sent immediately
}

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
    data->gyro_x = (int16_t)((g_raw_data[8] << 8) | g_raw_data[9]);
    data->gyro_y = (int16_t)((g_raw_data[10] << 8) | g_raw_data[11]);
    data->gyro_z = (int16_t)((g_raw_data[12] << 8) | g_raw_data[13]);

#if IMU_DEBUG_ENABLED == STD_ON
    // ESP_LOGI(g_TAG, "Accel: X=%.2f g, Y=%.2f g, Z=%.2f g | Gyro: X=%.2f °/s, Y=%.2f °/s, Z=%.2f °/s | Temp=%.2f °C",
    //              data->accel_x / ACCEL_SENS_2G,
    //              data->accel_y / ACCEL_SENS_2G,
    //              data->accel_z / ACCEL_SENS_2G,
    //              data->gyro_x / GYRO_SENS_250DPS,
    //              data->gyro_y / GYRO_SENS_250DPS,
    //              data->gyro_z / GYRO_SENS_250DPS,
    //              (data->temperature / 340.0f) + 36.53f);
      // Send each sensor value on its own teleplot line
    send_teleplot_data(data);
        
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

#endif // IMU_ENABLED