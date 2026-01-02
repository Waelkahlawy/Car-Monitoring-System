#ifndef CAMERA_HAL_H
#define CAMERA_HAL_H

#include "esp_camera.h"
#include "../../Config.h"

class CameraHAL {
public:
    static bool init();
    static camera_fb_t* capture();
    static void release(camera_fb_t* fb);
};
#endif