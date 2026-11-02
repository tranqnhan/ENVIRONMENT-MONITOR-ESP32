#include "esp_err.h"
#include "nvs_flash.h"

#include "p_nvs.h"


esp_err_t nvs_init(void) {
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    return err;

}

void nvs_save_string(const char *key, const char *value)
{
    nvs_handle_t handle;

    ESP_ERROR_CHECK(
        nvs_open("storage", NVS_READWRITE, &handle)
    );

    ESP_ERROR_CHECK(
        nvs_set_str(handle, key, value)
    );

    ESP_ERROR_CHECK(
        nvs_commit(handle)
    );

    nvs_close(handle);
}

esp_err_t nvs_read_string(const char *key, char *buffer, size_t buffer_size)
{
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t required_size = 0;

    err = nvs_get_str(handle, key, NULL, &required_size);

    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    if (required_size > buffer_size) {
        nvs_close(handle);
        return ESP_ERR_INVALID_SIZE;
    }

    err = nvs_get_str(handle, key, buffer, &required_size);

    nvs_close(handle);

    return err;
}