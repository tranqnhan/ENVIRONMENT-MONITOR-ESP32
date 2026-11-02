#pragma once

#include "esp_err.h"

#define NVS_USERNAME_KEY "USERNAME"
#define NVS_PASSWORD_KEY "PASSWORD"
#define NVS_SERVER_URL_KEY "SERVER_URL"

#define USERNAME_SIZE 9
#define PASSWORD_SIZE 9
#define SERVER_URL_SIZE 256

esp_err_t nvs_init(void);
void nvs_save_string(const char *key, const char *value);
esp_err_t nvs_read_string(const char *key, char *buffer, size_t buffer_size);