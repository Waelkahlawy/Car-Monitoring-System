#include "Camera_HAL.h"
#include <Arduino.h>

// AI THINKER PIN MAP
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

bool CameraHAL::init() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = CFG_CAM_XCLK_FREQ;
    config.pixel_format = CFG_CAM_PIXFORMAT;
    config.frame_size = CFG_CAM_FRAMESIZE;
    config.fb_count = CFG_CAM_FB_COUNT;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;

    if (esp_camera_init(&config) != ESP_OK) {
        return false;
    }

    configSensor();
    return true;
}

void CameraHAL::configSensor() {
    sensor_t * s = esp_camera_sensor_get();
    if (s) {
        s->set_brightness(s, 1);
        s->set_contrast(s, 1);
        s->set_saturation(s, 2);
        s->set_whitebal(s, 1);
        s->set_awb_gain(s, 1);
        s->set_wb_mode(s, 0);
        s->set_exposure_ctrl(s, 1);
        s->set_aec2(s, 1);
        s->set_gain_ctrl(s, 1);
    }
}

camera_fb_t* CameraHAL::getFrame() {
    return esp_camera_fb_get();
}

void CameraHAL::returnFrame(camera_fb_t* fb) {
    esp_camera_fb_return(fb);
}

void CameraHAL::drawBox(camera_fb_t *fb, int x, int y, int w, int h, uint16_t color) {
    if (!fb) return;
    int t = 3; // Thickness

    if(x < 0) x = 0; if(y < 0) y = 0;
    if(x + w > fb->width) w = fb->width - x;
    if(y + h > fb->height) h = fb->height - y;

    // Helper lambda to set pixel
    auto setPixel = [&](int px, int py) {
        if(px < fb->width && py < fb->height) {
            int idx = (py * fb->width + px) * 2;
            fb->buf[idx] = color >> 8;
            fb->buf[idx + 1] = color & 0xFF;
        }
    };

    for (int i = 0; i < t; i++) {
        for (int j = x; j < x + w; j++) {
            setPixel(j, y + i);             // Top
            setPixel(j, y + h - 1 - i);     // Bottom
        }
    }
    for (int i = 0; i < t; i++) {
        for (int j = y; j < y + h; j++) {
            setPixel(x + i, j);             // Left
            setPixel(x + w - 1 - i, j);     // Right
        }
    }
}