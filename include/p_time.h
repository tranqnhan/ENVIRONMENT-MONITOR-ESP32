#pragma once

#include "stdbool.h"
#include "esp_err.h"

#define UTC_TIME_FORMAT_SIZE 32

esp_err_t time_sntp_init(void);
esp_err_t time_get_utc_time(char *utc_time_str, size_t size);
bool time_is_ready(void);