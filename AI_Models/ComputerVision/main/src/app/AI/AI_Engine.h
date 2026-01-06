#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include "../../Config.h"
#include "esp_camera.h"

#if CFG_MODULE_AI == ENABLED
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

struct AIResult {
    const char* label;
    float score;
    bool valid;
    int index; 
};

class AIEngine {
public:
    bool init();
    AIResult run(camera_fb_t* fb);

private:
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;
    TfLiteTensor* output = nullptr;
    
    uint8_t *tensor_arena = nullptr;
    uint8_t *model_data = nullptr; 
    
    const tflite::Model* model = nullptr;
    
    bool loadModelFromSD();
    void preprocess(camera_fb_t* fb);
};
#endif
#endif