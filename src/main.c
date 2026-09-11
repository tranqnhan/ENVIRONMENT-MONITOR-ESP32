#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"

#include "i2c_setup.h"
#include "scd41.h"
#include "display.h"


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




// ---------------------------------------------------------
// Main
// ---------------------------------------------------------
void app_main(void)
{
    // Give USB Serial/JTAG time to connect
    vTaskDelay(pdMS_TO_TICKS(1000));

    esp_err_t err = i2c_init();
    handle_cmd(err, "i2c init");

    err = display_init();
    handle_cmd(err, "display init");

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
            //write_measurements(&scd41_m);
            display_measurements_task(&scd41_m);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
