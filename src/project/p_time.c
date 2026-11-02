#include "p_time.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_netif_sntp.h"

static const char *TAG = "NTP";

static bool _is_time_ready = false;


esp_err_t time_sntp_init(void)
{
    _is_time_ready = false;

    ESP_LOGI(TAG, "Initializing SNTP");

    esp_sntp_config_t config =
        ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

    ESP_ERROR_CHECK(esp_netif_sntp_init(&config));

    // Wait up to 10 seconds for synchronization
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) == ESP_OK) {
        ESP_LOGI(TAG, "Time synchronized!");
        _is_time_ready = true;
        return ESP_OK;
    } else {
        ESP_LOGW(TAG, "NTP synchronization timed out");
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t time_get_utc_time(char *utc_time_str, size_t size)
{
    time_t now;
    struct tm timeinfo;
    char strftime_buf[UTC_TIME_FORMAT_SIZE];

    time(&now);
    gmtime_r(&now, &timeinfo);

    strftime(strftime_buf, sizeof(strftime_buf),
             "%Y-%m-%dT%H:%M:%S", &timeinfo);

    if (size < UTC_TIME_FORMAT_SIZE) {
        return ESP_ERR_INVALID_SIZE;
    }

    snprintf(utc_time_str, size, "%s", strftime_buf);

    return ESP_OK;
}

bool time_is_ready(void) {
    return _is_time_ready;
}