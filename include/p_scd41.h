#pragma once

#include "esp_err.h"

#define SCD41_ADDR    0x62

typedef struct {
    uint16_t co2;
    float temperature;
    float humidity;
} measurement_t;


esp_err_t scd41_init();
esp_err_t scd41_start_periodic_measurement();
esp_err_t scd41_read_measurements(measurement_t* m);
esp_err_t scd41_data_ready(uint32_t *is_ready);