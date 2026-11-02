#include "p_http_client.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"

#include "p_scd41.h"

static esp_http_client_handle_t HTTP_API_CLIENT_HANDLE;

static void _http_client_message_to_json(
    const measurement_t *m, 
    const char *measured_at_utc, 
    const char *device_name, 
    char *json
);

void http_client_init(const char *server_url)
{
    const char *api_endpoint = "/data/add";
    const size_t api_url_size = strlen(api_endpoint) + strlen(server_url) + 1;
    char api_url[api_url_size];

    snprintf(api_url, api_url_size, "%s%s", server_url, api_endpoint);

    esp_http_client_config_t config = {
        .url = api_url,
        .method = HTTP_METHOD_POST,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
    };

    HTTP_API_CLIENT_HANDLE = esp_http_client_init(&config);
}


void http_client_send_sensor_data(    
    const measurement_t *m, 
    const char *measured_at_utc, 
    const char *device_name
) {
    char json_message[HTTP_MESSAGE_SIZE];
    _http_client_message_to_json(m, measured_at_utc, device_name, json_message);

    esp_http_client_set_post_field(
        HTTP_API_CLIENT_HANDLE,
        json_message,
        HTTP_MESSAGE_SIZE - 1
    );

    esp_err_t err = esp_http_client_perform(HTTP_API_CLIENT_HANDLE);

    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(HTTP_API_CLIENT_HANDLE);

        printf("HTTP status: %d\n", status);
    }
}


static void _http_client_message_to_json(
    const measurement_t *m, 
    const char *measured_at_utc, 
    const char *device_name, 
    char *json
) {
    const char *format = "{"
        "temperature:%5.2f,"
        "humidity:%5.2f,"
        "co2_level:%4d,"
        "measured_at:%s,"
        "device_name:%s"
    "}";

    snprintf(json, HTTP_MESSAGE_SIZE, format, 
        m->temperature, 
        m->humidity, 
        m->co2,
        measured_at_utc,
        device_name
    );
}