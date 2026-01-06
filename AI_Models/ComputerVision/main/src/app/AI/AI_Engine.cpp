#include "AI_Engine.h"

#if CFG_MODULE_AI == ENABLED


#include <FS.h>
#include <SD_MMC.h> 

static const char* kLabels[] = {
    "safe driving", "texting - right", "talking on the phone - right", 
    "texting - left", "talking on the phone - left", "operating the radio", 
    "Drinking", "reaching behind", "hair and makeup", "talking to passenger"
};

bool AIEngine::init() {
    
    tensor_arena = (uint8_t*) heap_caps_malloc(CFG_AI_ARENA_SIZE, MALLOC_CAP_SPIRAM);
    if (!tensor_arena) {
        Serial.println("AI Error: Arena Malloc Failed");
        return false;
    }

    
    if (!loadModelFromSD()) {
        return false;
    }

    
    model = tflite::GetModel(model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        Serial.println("AI Error: Model Schema Mismatch");
        return false;
    }

  
    static tflite::MicroMutableOpResolver<12> resolver;
    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddReshape();
    resolver.AddQuantize();
    resolver.AddDequantize();
    resolver.AddRelu();
    resolver.AddMaxPool2D();
    resolver.AddMean();
    resolver.AddPad();
    resolver.AddAdd();

    
    static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, CFG_AI_ARENA_SIZE);
    interpreter = &static_interpreter;

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        Serial.println("AI Error: AllocateTensors Failed");
        return false;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);
    
    Serial.println("AI Engine Initialized via SD Card!");
    return true;
}


bool AIEngine::loadModelFromSD() {
    Serial.println("Mounting SD Card...");
    
  
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("AI Error: SD Mount Failed");
        return false;
    }

    File file = SD_MMC.open(CFG_AI_MODEL_FILENAME);
    if (!file) {
        Serial.printf("AI Error: File %s not found\n", CFG_AI_MODEL_FILENAME);
        return false;
    }

    size_t size = file.size();
    Serial.printf("Model Size: %d bytes\n", size);

   
    model_data = (uint8_t*) heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
    if (!model_data) {
        Serial.println("AI Error: Failed to allocate RAM for Model");
        file.close();
        return false;
    }

    
    file.read(model_data, size);
    file.close();
    
    return true;
}

void AIEngine::preprocess(camera_fb_t* fb) {
    int w = input->dims->data[2];
    int h = input->dims->data[1];
    int8_t* in_data = input->data.int8; 

    for (int y = 0; y < h; y++) {
        int src_y = (y * fb->height) / h;
        for (int x = 0; x < w; x++) {
            int src_x = (x * fb->width) / w;
            int idx = (src_y * fb->width + src_x) * 2;
            
            uint16_t pixel = (fb->buf[idx] << 8) | fb->buf[idx + 1];
            uint8_t r = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t b = (pixel & 0x1F) << 3;

            int t_idx = (y * w + x) * 3;
            in_data[t_idx]     = (int8_t)(r - 128);
            in_data[t_idx + 1] = (int8_t)(g - 128);
            in_data[t_idx + 2] = (int8_t)(b - 128);
        }
    }
}

AIResult AIEngine::run(camera_fb_t* fb) {
    if (!fb) return {"", 0, false, -1}; 
    preprocess(fb);
    if (interpreter->Invoke() != kTfLiteOk) return {"Error", 0, false, -1};

    int8_t* out_data = output->data.int8;
    float max_score = 0;
    int best_idx = -1;
    TfLiteQuantizationParams params = output->params;

    for (int i = 0; i < 10; i++) {
        float val = (out_data[i] - params.zero_point) * params.scale;
        if (val > max_score) { max_score = val; best_idx = i; }
    }

    if (max_score > CFG_AI_CONF_THRESH) {
       
        return {kLabels[best_idx], max_score * 100, true, best_idx};
    }
    return {"Unknown", max_score * 100, false, -1};
}
#endif