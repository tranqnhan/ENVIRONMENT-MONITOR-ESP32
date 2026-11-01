#pragma once

#include "esp_err.h"

#define NVS_USERNAME_KEY "USERNAME"

esp_err_t nvs_init();
void nvs_save_string(const char *key, const char *value);
esp_err_t nvs_read_string(const char *key, char *buffer, size_t buffer_size);