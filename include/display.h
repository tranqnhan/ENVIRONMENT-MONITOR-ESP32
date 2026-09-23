#pragma once

#include "esp_err.h"
#include "scd41.h"
#include "ssd1306.h"

extern ssd1306_handle_t oled_dev_hdl;

esp_err_t display_init(void);
void display_measurements(const measurement_t *scd41_m);