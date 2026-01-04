#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>
#include <stdbool.h>
/* ================= Sensor Sources ================= */
typedef enum
{
    SRC_DHT11,       // DHT11 Temperature & Humidity
    SRC_IMU,         // MPU6050 Accelerometer/Gyro
    SRC_LDR,         // Light Sensor
    SRC_ULTRASONIC,  // Ultrasonic Distance Sensor
    SRC_GPS,         // GPS Module
    SRC_GSM,          // GSM Module / Signal
    SRC_FUSION,         // Sensor Fusion (Kalman Filter)
    SRC_PRED_MAINT      // Predictive Maintenance (ML Engine Diagnostics)
} SensorSource_t;

/* ================= Sensor Message ================= */
typedef struct
{
    SensorSource_t src;   // Source of the sensor message
        /* GSM */
        struct {
            char* gsm_alert; // GSM Alert message
        };
   
} SensorMsg_t;

#endif // SYSTEM_TYPES_H

