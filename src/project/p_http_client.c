#include "p_http_client.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include "esp_log.h"

#include "p_nvs.h"
#include "p_scd41.h"
#include "p_time.h"
#include "p_wifi.h"

extern const uint8_t local_server_crt_start[] asm("_binary_local_server_crt_start");

static char *TAG = "HTTP";

static esp_http_client_handle_t HTTP_API_CLIENT_HANDLE;
static bool _is_http_client_ready = false;
static char _username[USERNAME_SIZE];
static char _password[PASSWORD_SIZE];
static char _json_message[HTTP_REQUEST_SIZE];
static char _measured_at_utc[UTC_TIME_FORMAT_SIZE];

static void _http_client_message_to_json(const measurement_t *m, char *json);

void http_client_init()
{
    char server_url[SERVER_URL_SIZE];
    nvs_read_string(NVS_SERVER_URL_KEY, server_url, sizeof(server_url));
    ESP_LOGI(TAG, "server url: %s", server_url);
    
    const char *api_endpoint = "data/add";
    const size_t api_url_size = strlen(api_endpoint) + strlen(server_url) + 1;
    char api_url[api_url_size];

    nvs_read_string(NVS_USERNAME_KEY, _username, sizeof(_username));
    nvs_read_string(NVS_PASSWORD_KEY, _password, sizeof(_password));

    snprintf(api_url, api_url_size, "%s%s", server_url, api_endpoint);
    
    ESP_LOGI(TAG, "url: %s", api_url);

    esp_http_client_config_t config = {
        .url = api_url,
        .cert_pem = (const char *)local_server_crt_start,
        .method = HTTP_METHOD_POST,
        .keep_alive_enable = true
    };

    HTTP_API_CLIENT_HANDLE = esp_http_client_init(&config);

    ESP_ERROR_CHECK(
        esp_http_client_set_header(HTTP_API_CLIENT_HANDLE, "Content-Type", "application/json")
    );

    _is_http_client_ready = true;
}


void http_client_send_sensor_data(const measurement_t *m) {
    if (!(_is_http_client_ready && time_is_ready() && wifi_is_connected())) {
        ESP_LOGE(TAG, "Cannot send sensor data | Client ready? %d | Time ready? %d | Wi-Fi connected? %d\n", _is_http_client_ready, time_is_ready(), wifi_is_connected());
        return;
    } 
    
    time_get_utc_time(_measured_at_utc, sizeof(_measured_at_utc));
    ESP_LOGI(TAG, "utc time: %s", _measured_at_utc);

    _http_client_message_to_json(m, _json_message);

    esp_http_client_set_post_field(
        HTTP_API_CLIENT_HANDLE,
        _json_message,
        strlen(_json_message)
    );

    esp_err_t err = esp_http_client_perform(HTTP_API_CLIENT_HANDLE);

    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(HTTP_API_CLIENT_HANDLE);

        ESP_LOGI(TAG, "status: %d", status);
    }
}

static void _http_client_message_to_json(const measurement_t *m, char *json) 
{
    const char *format = "{"
        "\"temperature\":%5.2f,"
        "\"humidity\":%5.2f,"
        "\"co2Level\":%4d,"
        "\"measuredAt\":\"%s\","
        "\"monitorDeviceUsername\":\"%s\","
        "\"monitorDevicePassword\":\"%s\""
    "}";

    snprintf(json, HTTP_REQUEST_SIZE, format, 
        m->temperature, 
        m->humidity, 
        m->co2,
        _measured_at_utc,
        _username,
        _password
    );

    ESP_LOGI(TAG, "sending json: %s\n", json);
}