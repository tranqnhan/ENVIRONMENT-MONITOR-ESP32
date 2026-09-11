#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"

#include "ssd1306.h"

#include "i2c_setup.h"
#include "scd41.h"


const char *SCD41_TAG = "SCD41";


void handle_cmd(esp_err_t err, const char *cmd) {
    if (err != ESP_OK) {
        ESP_LOGE(SCD41_TAG, "%s command failed: %s.",
            cmd,
            esp_err_to_name(err));
    } else {
        ESP_LOGI(SCD41_TAG, "%s command success.",
            cmd
        );
    }
}


void write_measurements(const measurement_t *m) {
    ESP_LOGI(SCD41_TAG, "CO2: %d ppm | Temperature: %.2f C | Humidity: %.2f%%" ,
        m->co2, m->temperature, m->humidity
    );
}


// void scd41_task() {
//     esp_err_t err;

//     err = scd41_init();
//     handle_cmd(err, "Initialize");

//     err = scd41_start_periodic_measurement();
//     handle_cmd(err, "Start periodic measurement");
    
//     uint32_t is_data_ready;
//     measurement_t scd41_m;

//     while (1) {
//         is_data_ready = 0;
//         err = scd41_data_ready(&is_data_ready);
//         handle_cmd(err, "Data ready");

//         if (is_data_ready) {
//             err = scd41_read_measurement(&scd41_m);
//             handle_cmd(err, "Read");
//             write_measurements(&scd41_m);
//         }

//         vTaskDelay(pdMS_TO_TICKS(5000));
//     }
// }


ssd1306_handle_t oled_dev_hdl;

void display_measurements(const measurement_t *scd41_m) {
    ssd1306_clear_display(oled_dev_hdl, false);
    ssd1306_set_contrast(oled_dev_hdl, 0xff);
    
    char co2_s[11];
    char tmp_s[17];
    char hmd_s[17];

    sprintf(co2_s, "CO2 %d", scd41_m->co2);
    sprintf(tmp_s, "TMP %.2f", scd41_m->temperature);
    sprintf(hmd_s, "HMD %.2f", scd41_m->humidity);
    
    ssd1306_display_text(oled_dev_hdl, 0, co2_s, false);
    ssd1306_display_text(oled_dev_hdl, 1, tmp_s, false);
    ssd1306_display_text(oled_dev_hdl, 2, hmd_s, false);
}


// ---------------------------------------------------------
// Main
// ---------------------------------------------------------
void app_main(void)
{
    // Give USB Serial/JTAG time to connect
    vTaskDelay(pdMS_TO_TICKS(1000));

    esp_err_t err = i2c_init();
    handle_cmd(err, "i2c init");

    ssd1306_config_t dev_cfg = I2C_SSD1306_128x32_CONFIG_DEFAULT;
    ssd1306_init(i2c_bus_handle, &dev_cfg, &oled_dev_hdl);
    
    if (oled_dev_hdl == NULL) {
        ESP_LOGE("LCD", "ssd1306 handle init failed");
        assert(oled_dev_hdl);
    }

    err = scd41_init();
    handle_cmd(err, "scd41 init");

    err = scd41_start_periodic_measurement();
    handle_cmd(err, "scd41 start periodic measurement");
    
    uint32_t is_data_ready;
    measurement_t scd41_m;

    while (1) {
        is_data_ready = 0;
        err = scd41_data_ready(&is_data_ready);
        handle_cmd(err, "Data ready");

        if (is_data_ready) {
            err = scd41_read_measurement(&scd41_m);
            handle_cmd(err, "scd41 read");
            write_measurements(&scd41_m);
            display_measurements(&scd41_m);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
