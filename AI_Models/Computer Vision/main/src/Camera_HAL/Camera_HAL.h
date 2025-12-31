#ifndef CAMERA_HAL_H
#define CAMERA_HAL_H

#include "esp_camera.h"
#include "../Config.h"

class CameraHAL {
public:
    static bool init();
    static camera_fb_t* getFrame();
    static void returnFrame(camera_fb_t* fb);
    
    // Helper to draw bounding box directly on the frame buffer
    static void drawBox(camera_fb_t* fb, int x, int y, int w, int h, uint16_t color);

private:
    static void configSensor();
};

#endif