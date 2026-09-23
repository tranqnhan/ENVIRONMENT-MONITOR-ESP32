#include "driver/i2c_master.h"
#include "esp_err.h"

#include "p_i2c.h"

i2c_master_bus_handle_t i2c_bus_handle;

esp_err_t i2c_init() {

    // INITIALIZE I2C BUS
    i2c_master_bus_config_t i2c_bus_config = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    esp_err_t init_bus_err = i2c_new_master_bus(
        &i2c_bus_config,
        &i2c_bus_handle
    );

    return init_bus_err;
}

i2c_master_bus_handle_t* get_i2c_bus_handle() {
    return &i2c_bus_handle;
}