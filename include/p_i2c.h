#pragma once

#include "esp_err.h"
#include "driver/i2c_types.h"

#define I2C_SDA     GPIO_NUM_5
#define I2C_SCL     GPIO_NUM_4
#define I2C_PORT    I2C_NUM_0

esp_err_t i2c_init();
i2c_master_bus_handle_t* get_i2c_bus_handle();