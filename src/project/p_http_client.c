#include "p_http_client.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"

#include "p_nvs.h"
#include "p_scd41.h"
#include "p_time.h"
#include "p_wifi.h"

static esp_http_client_handle_t HTTP_API_CLIENT_HANDLE;
static bool _is_http_client_ready = false;
static char _username[USERNAME_SIZE];
static char _password[PASSWORD_SIZE];
static char _json_message[HTTP_MESSAGE_SIZE];
static char _measured_at_utc[UTC_TIME_FORMAT_SIZE];

static void _http_client_message_to_json(const measurement_t *m, const char *measured_at_utc, char *json);

void http_client_init()
{
    char server_url[SERVER_URL_SIZE];
    nvs_read_string(NVS_SERVER_URL_KEY, server_url, sizeof(server_url));

    const char *api_endpoint = "/data/add";
    const size_t api_url_size = strlen(api_endpoint) + strlen(server_url) + 1;
    char api_url[api_url_size];

    nvs_read_string(NVS_USERNAME_KEY, _username, sizeof(_username));
    nvs_read_string(NVS_USERNAME_KEY, _password, sizeof(_password));

    snprintf(api_url, api_url_size, "%s%s", server_url, api_endpoint);

    esp_http_client_config_t config = {
        .url = api_url,
        .method = HTTP_METHOD_POST,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
        .auth_type = HTTP_AUTH_TYPE_BASIC,
        .username = _username,
        .password = _password,
    };

    HTTP_API_CLIENT_HANDLE = esp_http_client_init(&config);

    _is_http_client_ready = true;
}


void http_client_send_sensor_data(const measurement_t *m) {
    if (!(_is_http_client_ready && time_is_ready() && wifi_is_connected())) {
        return;
    } 
    
    time_get_utc_time(_measured_at_utc, sizeof(_measured_at_utc));
    _http_client_message_to_json(m, _measured_at_utc, _json_message);

    esp_http_client_set_post_field(
        HTTP_API_CLIENT_HANDLE,
        _json_message,
        HTTP_MESSAGE_SIZE - 1
    );

    esp_err_t err = esp_http_client_perform(HTTP_API_CLIENT_HANDLE);

    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(HTTP_API_CLIENT_HANDLE);

        printf("HTTP status: %d\n", status);
    }
}


static void _http_client_message_to_json(const measurement_t *m, const char *measured_at_utc, char *json) 
{
    const char *format = "{"
        "temperature:%5.2f,"
        "humidity:%5.2f,"
        "co2_level:%4d,"
        "measured_at:%s"
    "}";

    snprintf(json, HTTP_MESSAGE_SIZE, format, 
        m->temperature, 
        m->humidity, 
        m->co2,
        measured_at_utc
    );
}