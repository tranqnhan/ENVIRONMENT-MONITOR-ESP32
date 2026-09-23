#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_err.h"

#include "i2c_setup.h"
#include "scd41.h"

i2c_master_dev_handle_t scd41_handle;

esp_err_t scd41_init() {
    // ADD SCD41 TO BUS
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SCD41_ADDR,
        .scl_speed_hz = 100000
    };

    esp_err_t add_device_err = i2c_master_bus_add_device (
        *get_i2c_bus_handle(),
        &dev_config,
        &scd41_handle
    );

    return add_device_err;
}


esp_err_t scd41_start_periodic_measurement() {
    const uint8_t start_periodic_measurement[2] = {0x21, 0xb1};

    esp_err_t spm_err = i2c_master_transmit(
        scd41_handle,
        start_periodic_measurement,
        2,
        1000    
    );

    return spm_err;
}


esp_err_t scd41_read_measurements(measurement_t* m) {
    const uint8_t read_measurement_cmd[2] = {0xec, 0x05};
    esp_err_t read_err = i2c_master_transmit(
        scd41_handle,
        read_measurement_cmd,
        2,
        1000    
    );

    if (read_err != ESP_OK) return read_err;

    uint8_t data[9] = {0};
    esp_err_t receive_err = i2c_master_receive(
        scd41_handle,
        data,
        9,
        1000
    );


    if (receive_err != ESP_OK) return receive_err;

    m->co2 = data[0] << 8 | data[1];
    
    m->temperature = -45.0 + 175.0 * (
        (float)(data[3] << 8 | data[4]) / 65535.0
    );

    m->humidity = 100.0 * ((float)(data[6] << 8 | data[7]) / 65535.0);

    return receive_err;
}


esp_err_t scd41_data_ready(uint32_t *is_ready) {
    const uint8_t data_ready_cmd[2] = {0xe4, 0xb8};
    esp_err_t data_ready_err = i2c_master_transmit(
        scd41_handle,
        data_ready_cmd,
        2,
        1000
    );

    if (data_ready_err != ESP_OK) return data_ready_err;

    uint8_t data[3] = {0};
    esp_err_t receive_err = i2c_master_receive(
        scd41_handle,
        data,
        3,
        1000
    );

    *is_ready = (data[0] << 16) | (data[1] << 8) | (data[0]);

    return receive_err;
}