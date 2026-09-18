#pragma once

#include "esp_err.h"

#define DEBUG 1

extern const char *TAG;

void handle_cmd(esp_err_t err, const char *cmd);

