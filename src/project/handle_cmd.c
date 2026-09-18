#include "handle_cmd.h"
#include "esp_err.h"
#include "esp_log.h"

const char *TAG = "ESP32 DEVICE";

void handle_cmd(esp_err_t err, const char *cmd) {
    if (!DEBUG) return;

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s command failed: %s.",
            cmd,
            esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "%s command success.",
            cmd
        );
    }
}
