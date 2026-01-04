// predictive_maintenance.cpp

#include "predictive_maintenance.h"

#if PREDICTIVE_MAINTENANCE_ENABLED == STD_ON

#include <math.h>
#include <string.h>

#include "esp_log.h"

#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "engine_nn_quant_three_features.h"  // Your quantized model
#include "../IMU/imu.h"

static const char* TAG = "PRED_MAINT";

// Keep error reporter for potential future use / debugging
static tflite::MicroErrorReporter micro_error_reporter;

static tflite::MicroMutableOpResolver<7> resolver;

constexpr size_t kTensorArenaSize = 5 * 1024; 
static uint8_t tensor_arena[kTensorArenaSize] __attribute__((aligned(16)));

static tflite::MicroInterpreter* interpreter = nullptr;

// Feature collection window
#define WINDOW_SIZE 200
static Imu_DataType imu_window[WINDOW_SIZE];

void Predictive_Maintenance_Init(void)
{
    resolver.AddFullyConnected();
    resolver.AddQuantize();
    resolver.AddDequantize();
    resolver.AddRelu();
    resolver.AddLogistic(); 
    resolver.AddReshape();
    resolver.AddSoftmax();  

    const tflite::Model* model = tflite::GetModel(engine_nn_quant_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        ESP_LOGE(TAG, "Model schema version mismatch");
        return;
    }

    // Correct constructor for esp-tflite-micro 1.3.5: only 4 arguments
    static tflite::MicroInterpreter static_interpreter(
        model,
        resolver,
        tensor_arena,
        kTensorArenaSize
    );

    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        ESP_LOGE(TAG, "AllocateTensors() failed - increase kTensorArenaSize");
        interpreter = nullptr;
        return;
    }

    ESP_LOGI(TAG, "Predictive Maintenance Initialized (Arena used: %zu bytes)",
             interpreter->arena_used_bytes());
}

static void compute_features(Predictive_FeaturesType* features)
{
    int i, j;

    for (i = 0; i < WINDOW_SIZE; i++) {
        Imu_Main(&imu_window[i], MPU6050_ADDR2 );
    }

    float magnitudes[WINDOW_SIZE];
    float temperatures[WINDOW_SIZE];

    for (i = 0; i < WINDOW_SIZE; i++) {
        float ax = imu_window[i].accel_x;
        float ay = imu_window[i].accel_y;
        float az = imu_window[i].accel_z;
        magnitudes[i] = sqrtf(ax*ax + ay*ay + az*az);
        temperatures[i] = imu_window[i].temperature;
    }

    float sum_sq = 0.0f;
    for (i = 0; i < WINDOW_SIZE; i++) {
        sum_sq += magnitudes[i] * magnitudes[i];
    }
    features->rms_vibration = sqrtf(sum_sq / WINDOW_SIZE);

    float mean = 0.0f;
    for (i = 0; i < WINDOW_SIZE; i++) {
        mean += magnitudes[i];
    }
    mean /= WINDOW_SIZE;

    float sum_dev2 = 0.0f;
    float sum_dev4 = 0.0f;
    for (i = 0; i < WINDOW_SIZE; i++) {
        float dev = magnitudes[i] - mean;
        sum_dev2 += dev * dev;
        sum_dev4 += dev * dev * dev * dev;
    }
    float variance = sum_dev2 / WINDOW_SIZE;
    if (variance > 1e-6f) {
        features->kurtosis = (sum_dev4 / WINDOW_SIZE) / (variance * variance) - 3.0f;
    } else {
        features->kurtosis = 0.0f;
    }

    float temps_sorted[WINDOW_SIZE];
    memcpy(temps_sorted, temperatures, sizeof(temperatures));

    for (i = 0; i < WINDOW_SIZE - 1; i++) {
        for (j = i + 1; j < WINDOW_SIZE; j++) {
            if (temps_sorted[i] > temps_sorted[j]) {
                float temp = temps_sorted[i];
                temps_sorted[i] = temps_sorted[j];
                temps_sorted[j] = temp;
            }
        }
    }
    features->median_temperature = temps_sorted[WINDOW_SIZE / 2];

    ESP_LOGI(TAG, "Features: RMS=%.3f, Kurtosis=%.3f, Median Temp=%.2f°C",
             features->rms_vibration, features->kurtosis, features->median_temperature);
}

void Predictive_Maintenance_Run(Predictive_FeaturesType* features,
                                Predictive_OutputType* output)
{
    if (!interpreter) {
        ESP_LOGE(TAG, "Interpreter not initialized");
        return;
    }

    compute_features(features);

    TfLiteTensor* input_tensor = interpreter->input(0);

    static const float feature_mean[3] = {1.68595058f, 4.4131862f, 15.39738587f};
    static const float feature_std[3]  = {1.5730099f,  7.29535932f, 19.69827512f};

    float normalized[3];
    normalized[0] = (features->rms_vibration     - feature_mean[0]) / feature_std[0];
    normalized[1] = (features->kurtosis          - feature_mean[1]) / feature_std[1];
    normalized[2] = (features->median_temperature - feature_mean[2]) / feature_std[2];

    float scale = input_tensor->params.scale;
    int32_t zero_point = input_tensor->params.zero_point;

    int i;
    for (i = 0; i < 3; i++) {
        float scaled_value = normalized[i] / scale + zero_point;
        input_tensor->data.int8[i] = (int8_t)roundf(scaled_value);
    }

    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        ESP_LOGE(TAG, "Invoke failed");
        return;
    }

    TfLiteTensor* output_tensor = interpreter->output(0);
    float probability = (float)(output_tensor->data.int8[0] - output_tensor->params.zero_point) *
                        output_tensor->params.scale;

    output->prediction = (probability > 0.5f) ? 1 : 0;
    output->confidence = probability;

    ESP_LOGI(TAG, "Engine Status: %s (Confidence: %.2f)",
             output->prediction ? "Faulty" : "Healthy", output->confidence);
}

#else

void Predictive_Maintenance_Init(void) {}
void Predictive_Maintenance_Run(Predictive_FeaturesType* features, Predictive_OutputType* output)
{
    (void)features;
    (void)output;
}

#endif // PREDICTIVE_MAINTENANCE_ENABLED