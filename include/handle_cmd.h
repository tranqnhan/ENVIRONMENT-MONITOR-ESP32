#pragma once

#include "esp_err.h"

#define DEBUG 1

extern const char *DEVICE_NAME;

int handle_cmd(esp_err_t err, const char *cmd);
int handle_cmd_int(int err, const char *cmd);

