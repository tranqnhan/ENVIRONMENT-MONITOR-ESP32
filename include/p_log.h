#pragma once

#include "esp_err.h"

#define DEBUG 1

extern const char *DEVICE_NAME;

int log_command(esp_err_t err, const char *cmd);
int log_command_int(int err, const char *cmd);

