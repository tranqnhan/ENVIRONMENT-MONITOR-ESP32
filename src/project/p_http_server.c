#include "esp_err.h"
#include "esp_http_server.h"
#include "p_log.h"

#include "p_http_server.h"

/* http server:
    - acts as a WiFi provisioning system.
*/

static void http_server_register_uris(httpd_handle_t server_handle);

void http_server_init() {
    esp_err_t err;
    httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server_handle = NULL;

    err = httpd_start(&server_handle, &server_config);
    log_command(err, "httpd_start");

    if (err != ESP_OK) return;

    http_server_register_uris(server_handle);
}

/* REGISTER URI */
static esp_err_t RESP_hello(httpd_req_t *req) {
    const char *resp_str = "<h1> HELLO WORLD </h1>";
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t URI_hello = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = RESP_hello,
    .user_ctx = NULL
};
/* --------- */

static void http_server_register_uris(httpd_handle_t server_handle) {
    esp_err_t err;

    err = httpd_register_uri_handler(server_handle, &URI_hello);
    log_command(err, "httpd_register_uri_handler(server_handle, &URI_hello)");
    

}