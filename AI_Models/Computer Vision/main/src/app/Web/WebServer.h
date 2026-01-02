#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_http_server.h"
#include "../../Config.h"

class WebServer {
public:
    static void init();
private:
    static esp_err_t stream_handler(httpd_req_t *req);
};

#endif