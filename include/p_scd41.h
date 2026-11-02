#pragma once

#include <stdbool.h>
#include "esp_err.h"

#define SCD41_ADDR    0x62

typedef struct {
    uint16_t co2;
    float temperature;
    float humidity;
} measurement_t;


esp_err_t scd41_init(void);
esp_err_t scd41_start_periodic_measurement(void);
esp_err_t scd41_read_measurements(measurement_t* m);
esp_err_t scd41_data_ready(bool *is_ready);