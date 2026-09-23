#include "handle_cmd.h"
#include "esp_err.h"
#include "esp_log.h"

const char *DEVICE_NAME = "ESP32 DEVICE";

int handle_cmd(esp_err_t err, const char *cmd) {
    if (!DEBUG) return err;

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

int handle_cmd_int(int err, const char *cmd) {
    if (!DEBUG) return err;

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
