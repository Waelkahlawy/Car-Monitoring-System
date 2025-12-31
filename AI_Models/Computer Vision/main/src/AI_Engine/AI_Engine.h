#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include "../Config.h"
#include "esp_camera.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

// If Flash load is selected, include the array file
#if (CFG_MODEL_LOAD_FROM_SD == STD_OFF)
    #include "model_data.h"
#endif

struct PredictionResult {
    const char* label;
    float score;
    bool valid;
};

class AIEngine {
public:
    bool begin();
    PredictionResult runInference(camera_fb_t* fb);

private:
    // TFLite components
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;
    TfLiteTensor* output = nullptr;
    
    // Memory
    uint8_t *model_data_buffer = nullptr;
    uint8_t *tensor_arena = nullptr;

    bool loadModel();
    void processImage(camera_fb_t* fb);
};

#endif