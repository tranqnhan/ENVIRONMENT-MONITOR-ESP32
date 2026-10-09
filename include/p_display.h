#pragma once

#include "esp_err.h"
#include "ssd1306.h"

#include "p_scd41.h"

extern ssd1306_handle_t oled_dev_hdl;

esp_err_t display_init(void);
void display_measurements(const measurement_t *scd41_m);
void display_wifi_provision(const char *username, const char *password, const uint32_t seconds_remaining, const char *message);
void display_wifi_provision_time(const uint32_t seconds_remaining);
void display_wifi_provision_message(const char *message);
