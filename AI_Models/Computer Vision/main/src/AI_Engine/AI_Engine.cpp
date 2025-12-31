#include "AI_Engine.h"
#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>

bool AIEngine::begin() {
    if (!loadModel()) {
        Serial.println("ERR: Failed to load model");
        return false;
    }

    // Allocate Tensor Arena
    tensor_arena = (uint8_t*) heap_caps_malloc(CFG_AI_ARENA_SIZE, MALLOC_CAP_SPIRAM);
    if (!tensor_arena) {
        Serial.println("ERR: Failed to allocate tensor arena in PSRAM");
        return false;
    }

    // Define Ops
    static tflite::MicroMutableOpResolver<16> resolver;
    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddReshape();
    resolver.AddMaxPool2D();
    resolver.AddAveragePool2D();
    resolver.AddQuantize();
    resolver.AddDequantize();
    resolver.AddRelu();
    resolver.AddRelu6();
    resolver.AddAdd();
    resolver.AddMul();
    resolver.AddPad();
    resolver.AddMean();

    // Create Interpreter
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, CFG_AI_ARENA_SIZE);
    interpreter = &static_interpreter;

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        Serial.println("ERR: AllocateTensors failed");
        return false;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);
    
    Serial.printf("AI Engine Started. Input: %dx%d\n", input->dims->data[2], input->dims->data[1]);
    return true;
}

bool AIEngine::loadModel() {
#if (CFG_MODEL_LOAD_FROM_SD == STD_ON)
    // --- LOAD FROM SD CARD ---
    if(!SD_MMC.begin(CFG_SD_MOUNT_POINT, true)){
        Serial.println("SD Mount Failed");
        return false;
    }
    
    File file = SD_MMC.open(CFG_MODEL_FILENAME);
    if(!file){
        Serial.println("Model file missing on SD");
        return false;
    }

    size_t model_size = file.size();
    model_data_buffer = (uint8_t*) heap_caps_malloc(model_size, MALLOC_CAP_SPIRAM);
    if(!model_data_buffer) {
        file.close();
        return false;
    }

    if(file.read(model_data_buffer, model_size) != model_size) {
        file.close();
        return false;
    }
    file.close();
    
    model = tflite::GetModel(model_data_buffer);
#else
    // --- LOAD FROM FLASH (Header File) ---
    // Assumes model_data array exists in model_data.h
    model = tflite::GetModel(model_data);
#endif

    if (model->version() != TFLITE_SCHEMA_VERSION) {
        Serial.println("Schema version mismatch");
        return false;
    }
    return true;
}

void AIEngine::processImage(camera_fb_t* fb) {
    // Basic Center Crop and Resize Logic to Input Tensor
    // Assuming RGB565 input from camera, converting to RGB888 for model
    
    int h = CFG_AI_INPUT_HEIGHT;
    int w = CFG_AI_INPUT_WIDTH;
    int min_side = (fb->width < fb->height) ? fb->width : fb->height;
    int crop_x = (fb->width - min_side) / 2;
    int crop_y = (fb->height - min_side) / 2;

    if (input->type == kTfLiteUInt8 || input->type == kTfLiteInt8) {
        // Handle Quantized Input
        // Note: Implementation depends on specific normalization needs of the model.
        // This is a generic standard resize loop similar to original code.
        uint8_t* in_ptr = input->data.uint8;
        
        for (int y = 0; y < h; y++) {
            int src_y = crop_y + (y * min_side / h);
            for (int x = 0; x < w; x++) {
                int src_x = crop_x + (x * min_side / w);
                src_y = (src_y >= fb->height) ? fb->height-1 : src_y;
                src_x = (src_x >= fb->width) ? fb->width-1 : src_x;

                int pix_idx = (src_y * fb->width + src_x) * 2;
                uint16_t rgb565 = (fb->buf[pix_idx] << 8) | fb->buf[pix_idx + 1];
                
                // Convert to RGB888
                in_ptr[(y*w+x)*3 + 0] = ((rgb565 >> 11) & 0x1F) * 255 / 31; // R
                in_ptr[(y*w+x)*3 + 1] = ((rgb565 >> 5) & 0x3F) * 255 / 63;  // G
                in_ptr[(y*w+x)*3 + 2] = (rgb565 & 0x1F) * 255 / 31;         // B
            }
        }
    } else if (input->type == kTfLiteFloat32) {
        // Implement Float logic if needed
    }
}

PredictionResult AIEngine::runInference(camera_fb_t* fb) {
    processImage(fb);

    if (interpreter->Invoke() != kTfLiteOk) {
        return {"Error", 0.0, false};
    }

    // Process Output
    float max_score = 0;
    int best_idx = -1;
    TfLiteQuantizationParams params = output->params;

    if (output->type == kTfLiteUInt8) {
        uint8_t* results = output->data.uint8;
        for (int i = 0; i < CFG_NUM_LABELS; i++) {
            float prob = (results[i] - params.zero_point) * params.scale;
            if (prob > max_score) { max_score = prob; best_idx = i; }
        }
    } else if (output->type == kTfLiteInt8) {
        int8_t* results = output->data.int8;
        for (int i = 0; i < CFG_NUM_LABELS; i++) {
            float prob = (results[i] - params.zero_point) * params.scale;
            if (prob > max_score) { max_score = prob; best_idx = i; }
        }
    } else {
        float* results = output->data.f;
        for (int i = 0; i < CFG_NUM_LABELS; i++) {
            if (results[i] > max_score) { max_score = results[i]; best_idx = i; }
        }
    }

    PredictionResult res;
    if (best_idx >= 0 && max_score > CFG_AI_CONFIDENCE_THR) {
        res.label = kLabels[best_idx];
        res.score = max_score;
        res.valid = true;
    } else {
        res.label = "Unsure";
        res.score = max_score;
        res.valid = false;
    }
    return res;
}