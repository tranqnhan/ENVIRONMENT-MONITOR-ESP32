#pragma once

#include "esp_err.h"

// TODO: create p_device_info and get_device_name()
extern const char *DEVICE_NAME;

void log_command(esp_err_t err, const char *cmd);
void log_command_int(int err, const char *cmd);

