#include "Camera_HAL.h"

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
    
    // A frequency of 20MHz is the most stable for the image (reduces stuttering)
    config.xclk_freq_hz = 20000000;
    
    config.pixel_format = PIXFORMAT_RGB565; 
    
    // --- 1. The middle ground between resolution and speed ---
    // VGA (640x480) is too heavy and slow.
    // QVGA (320x240) is fast but has low resolution.
    // CIF (400x296) is the best middle ground (acceptable resolution + high speed).
    config.frame_size = FRAMESIZE_CIF; 
    
    // --- 2. Quality Control ---
    // Number 12 is the "Sweet Spot" (less than 10 causes extreme slowness, and more than 15 reduces quality)
    config.jpeg_quality = 12; 
    config.fb_count = 1;

    // Enable external memory to speed up video (buffering)
    if(psramFound()){
        config.fb_count = 2; 
        config.grab_mode = CAMERA_GRAB_LATEST; // Always takes the latest image to prevent lag
    } else {
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    if (esp_camera_init(&config) != ESP_OK) return false;

    // --- 3. De-Noising ---
    sensor_t * s = esp_camera_sensor_get();
    if (s != NULL) {
        // Reset contrast and brightness to normal (0) to eliminate blur
        s->set_brightness(s, 0);  
        s->set_contrast(s, 0);    
        s->set_saturation(s, 0);  
        
        // Enable automatic features (very important)
        s->set_whitebal(s, 1);       // Automatic color balance
        s->set_awb_gain(s, 1);       
        s->set_wb_mode(s, 0);        // Auto
        
        // Exposure settings
        s->set_exposure_ctrl(s, 1);  
        s->set_aec2(s, 1);           // DSP light processing (greatly improves image)
        s->set_ae_level(s, 0);       
        s->set_aec_value(s, 300);    
        s->set_gain_ctrl(s, 1);     // Reduce light sensitivity to reduce noise

        // Disable lens correction because it consumes a lot of processor power and may cause slowdowns
        s->set_lenc(s, 0); 
        
        // Enable the "BPC" (Black Pixel Correction) feature to remove random black dots
        s->set_bpc(s, 1); 
        
        // Enable "WPC" (White Pixel Correction) to remove random white spots
        s->set_wpc(s, 1);
        
        // Enable Raw GMA to improve image smoothness
        s->set_raw_gma(s, 1);
    }

    return true;
}

camera_fb_t* CameraHAL::capture() {
    return esp_camera_fb_get();
}

void CameraHAL::release(camera_fb_t* fb) {
    esp_camera_fb_return(fb);
}