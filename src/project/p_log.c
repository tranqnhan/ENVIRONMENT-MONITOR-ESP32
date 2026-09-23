#include "esp_err.h"
#include "esp_log.h"

#include "p_log.h"

const char *DEVICE_NAME = "ENV MONITOR";

int log_command(esp_err_t err, const char *cmd) {
    if (err != ESP_OK) {
        if (DEBUG) {
            ESP_LOGE(DEVICE_NAME, "%s command failed: %s.",
                cmd,
                esp_err_to_name(err));
        }
        return 1;
    } else {
        if (DEBUG) {
            ESP_LOGI(DEVICE_NAME, "%s command success.",
                cmd
            );
        }
        return 0;
    }
}

int log_command_int(int err, const char *cmd) {
    if (err != 0) {
        if (DEBUG) {
            ESP_LOGE(DEVICE_NAME, "%s command failed: %d.",
                cmd,
                err);
        }
        return 1;
    } else {
        if (DEBUG) {
            ESP_LOGI(DEVICE_NAME, "%s command success.",
                cmd
            );
        }
        return 0;
    }
}
