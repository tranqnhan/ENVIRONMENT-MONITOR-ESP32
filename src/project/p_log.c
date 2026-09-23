#include "esp_err.h"
#include "esp_log.h"

#include "p_log.h"

#define DEBUG 1

const char *DEVICE_NAME = "ENV MONITOR";

void log_command(esp_err_t err, const char *cmd) {
    if (!DEBUG) return;

    if (err != ESP_OK) {
        ESP_LOGE(DEVICE_NAME, "%s command failed: %s.", cmd, esp_err_to_name(err));
    } else {
        ESP_LOGI(DEVICE_NAME, "%s command success.", cmd);
    }
}

void log_command_int(int err, const char *cmd) {
    if (!DEBUG) return;
    
    if (err != 0) {
        ESP_LOGE(DEVICE_NAME, "%s command failed: %d.", cmd, err);
    } else {
        ESP_LOGI(DEVICE_NAME, "%s command success.", cmd);
    }
}
