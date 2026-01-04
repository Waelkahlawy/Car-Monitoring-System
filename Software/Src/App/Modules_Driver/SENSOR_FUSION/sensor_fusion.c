#include "sensor_fusion.h"

#if SENSOR_FUSION_ENABLED == STD_ON

#include "esp_log.h"
#include <math.h>
//#include "esp_timer.h" // For precise timing

static const char *TAG = TAG_SENSOR_FUSION;

/* Global Kalman Filter Variables */
static float pos_x, vel_x;
static float pos_y, vel_y;

// P: Error Covariance Matrix (Uncertainty in our estimate)
static float P[4][4];  
// Q: Process Noise Covariance (Trust in IMU model)
static float Q_noise[4][4];  
// R: Measurement Noise Covariance (Trust in GPS data)
static float R_noise[2][2];  

/**
 * Step 1: Fusion_Init
 * Sets the initial state and seeds the uncertainty matrices.
 */
void Fusion_Init(void) 
{
    // Initialize State: Assume starting at relative origin (0,0) and stationary
    pos_x = 0.0f;
    vel_x = 0.0f;
    pos_y = 0.0f;
    vel_y = 0.0f;

    // Initialize P: High values on diagonal (1.0) indicate high initial uncertainty
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            P[i][j] = (i == j) ? 1.0f : 0.0f; 
        }
    }

    // Initialize Q: Low values (0.01) represent high trust in the IMU's short-term physics
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Q_noise[i][j] = (i == j) ? 0.01f : 0.0f;
        }
    }

    // Initialize R: Higher values (1.0) represent the expected variance/noise of GPS
    R_noise[0][0] = 1.0f; 
    R_noise[1][1] = 1.0f;

    ESP_LOGI(TAG, "Sensor Fusion Initialized: State and Matrices ready.");
}

/**
 * Step 2: Fusion_Process
 * Integrates IMU acceleration and corrects with GPS coordinates.
 */
void Fusion_Main(Imu_DataType *imu_data, Gps_DataType *gps_data, Fusion_DataType *fused_result) 
{
    // A. Calculate Time Delta (dt)
    uint32_t now = esp_log_timestamp(); 
    float dt = (now - fused_result->last_timestamp) / 1000.0f; // Convert ms to seconds
    fused_result->last_timestamp = now;

    if (dt <= 0 || dt > 1.0) dt = 0.1f; // Clamp dt to a safe default if leap occurs

    // B. PREDICTION STEP (IMU)
    // Use Newton's equations: s = s0 + v*dt + 0.5*a*dt^2
    pos_x += (vel_x * dt) + (0.5f * imu_data->accel_x * dt * dt);
    pos_y += (vel_y * dt) + (0.5f * imu_data->accel_y * dt * dt);

    // Update velocity: v = v0 + a*dt
    vel_x += imu_data->accel_x * dt;
    vel_y += imu_data->accel_y * dt;

    // Expand Uncertainty P: P = P + Q
    for(int i = 0; i < 4; i++)
    {
        P[i][i] += Q_noise[i][i];
    }

    // C. CORRECTION STEP (GPS)
    // Only update if the GPS driver reports a valid fix
    if (gps_data != '\0' && gps_data->valid)
    {
        // Calculate Kalman Gain K = P / (P + R)
        float K_x = P[0][0] / (P[0][0] + R_noise[0][0]);
        float K_y = P[2][2] / (P[2][2] + R_noise[1][1]);

        // Adjust state based on the difference between GPS and Prediction
        pos_x = pos_x + K_x * (gps_data->latitude - pos_x);
        pos_y = pos_y + K_y * (gps_data->longitude - pos_y);

        // Update velocity (Simple adjustment based on gain)
        vel_x = vel_x + (K_x / dt) * (gps_data->latitude - pos_x); 
        
        // Reduce Uncertainty P: P = (1 - K) * P
        P[0][0] = (1.0f - K_x) * P[0][0];
        P[2][2] = (1.0f - K_y) * P[2][2];
        
        ESP_LOGD(TAG, "GPS Correction applied. Lat: %.6f, Lon: %.6f", pos_x, pos_y);
    }

    // D. Update Output Structure
    fused_result->fused_lat  = pos_x;
    fused_result->fused_lon  = pos_y;
    fused_result->velocity_x = vel_x;
    fused_result->velocity_y = vel_y;
}

#endif /* SENSOR_FUSION_ENABLE */