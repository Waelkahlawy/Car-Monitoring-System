#include "WebServer.h"
#if CFG_MODULE_WEBSERVER == ENABLED
#include "../../hal/Camera/Camera_HAL.h"
#include <SD_MMC.h>  
#include <FS.h>
#include "img_converters.h" 

extern String g_lastLabel;
extern float g_lastScore;
extern SemaphoreHandle_t xDataMutex;

httpd_handle_t web_httpd = NULL;
httpd_handle_t stream_httpd = NULL;
httpd_handle_t api_httpd = NULL;


esp_err_t sendFile(httpd_req_t *req, String filename, String mimeType) {
    File file = SD_MMC.open(filename, "r");
    if (!file) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, mimeType.c_str());
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    
    char chunk[1024]; 
    size_t chunksize;
    while (file.available()) {
        chunksize = file.read((uint8_t *)chunk, 1024);
        if (chunksize > 0) httpd_resp_send_chunk(req, chunk, chunksize);
    }
    file.close();
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}


esp_err_t index_handler(httpd_req_t *req) { return sendFile(req, "/data/main.html", "text/html"); }
esp_err_t css_handler(httpd_req_t *req) { return sendFile(req, "/data/main.css", "text/css"); }
esp_err_t js_handler(httpd_req_t *req) { return sendFile(req, "/data/main.js", "application/javascript"); }


esp_err_t api_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    
    httpd_resp_set_hdr(req, "Connection", "close"); 
    
    String label = "Init";
    float score = 0.0;
    if (xDataMutex != NULL) {
        if(xSemaphoreTake(xDataMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            label = g_lastLabel;
            score = g_lastScore;
            xSemaphoreGive(xDataMutex);
        }
    }
    char json_response[128];
    snprintf(json_response, 128, "{\"label\":\"%s\",\"score\":%.1f}", label.c_str(), score);
    httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


esp_err_t WebServer::stream_handler(httpd_req_t *req) {
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    char part_buf[64];
    static const char* _STREAM_BOUNDARY = "\r\n--123456789000000000000987654321\r\n";
    static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

    res = httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=123456789000000000000987654321");
    if(res != ESP_OK) return res;

    while(true) {
        fb = CameraHAL::capture(); 
        if (!fb) { res = ESP_FAIL; break; }

        uint8_t * jpg_buf = NULL;
        size_t jpg_len = 0;
        
       
        bool converted = frame2jpg(fb, 15, &jpg_buf, &jpg_len);
        CameraHAL::release(fb); 
        
        if(!converted) { res = ESP_FAIL; break; }

        if(res == ESP_OK) res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        if(res == ESP_OK) {
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, jpg_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK) res = httpd_resp_send_chunk(req, (const char *)jpg_buf, jpg_len);
        
        free(jpg_buf);
        if(res != ESP_OK) break;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
    return res;
}

void WebServer::init() {
    
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.stack_size = 8192; 
    config.max_open_sockets = 3; 
    config.lru_purge_enable = true; 

    httpd_uri_t index_uri = { "/", HTTP_GET, index_handler, NULL };
    httpd_uri_t css_uri = { "/main.css", HTTP_GET, css_handler, NULL };
    httpd_uri_t js_uri = { "/main.js", HTTP_GET, js_handler, NULL };

    if (httpd_start(&web_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(web_httpd, &index_uri);
        httpd_register_uri_handler(web_httpd, &css_uri);
        httpd_register_uri_handler(web_httpd, &js_uri);
    }

  
    httpd_config_t stream_config = HTTPD_DEFAULT_CONFIG();
    stream_config.server_port = 81;
    stream_config.ctrl_port = 81;
    stream_config.stack_size = 8192;
    stream_config.max_open_sockets = 1; 
    stream_config.lru_purge_enable = true;

    httpd_uri_t stream_uri = { "/stream", HTTP_GET, stream_handler, NULL };
    
    if (httpd_start(&stream_httpd, &stream_config) == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }

    
    httpd_config_t api_config = HTTPD_DEFAULT_CONFIG();
    api_config.server_port = 82;
    api_config.ctrl_port = 82;
    api_config.stack_size = 4096;
    api_config.max_open_sockets = 2; 
    api_config.lru_purge_enable = true; 

    httpd_uri_t api_uri = { "/", HTTP_GET, api_handler, NULL };
    
    if (httpd_start(&api_httpd, &api_config) == ESP_OK) {
        httpd_register_uri_handler(api_httpd, &api_uri);
    }
}
#endif