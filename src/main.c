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

#include "handle_cmd.h"


void setup() {
    esp_err_t err = i2c_init();
    handle_cmd(err, "i2c init");

    err = display_init();
    handle_cmd(err, "display init");

    err = scd41_init();
    handle_cmd(err, "scd41 init");

    err = scd41_start_periodic_measurement();
    handle_cmd(err, "scd41 start periodic measurement");
}


void environment_monitor_task(void *pvParameter) {
    uint32_t is_data_ready;
    measurement_t scd41_m;
    esp_err_t err;

    while (1) {
        is_data_ready = 0;
        err = scd41_data_ready(&is_data_ready);
        handle_cmd(err, "Data ready");

        if (is_data_ready) {
            err = scd41_read_measurement(&scd41_m);
            handle_cmd(err, "scd41 read");
            display_measurements_task(&scd41_m);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }

}


// ---------------------------------------------------------
// Main
// ---------------------------------------------------------
void app_main(void)
{
    // Give USB Serial/JTAG time to connect
    vTaskDelay(pdMS_TO_TICKS(1000));

    setup();

    xTaskCreate(&environment_monitor_task, "Environment Monitor", 1500, NULL, 5, NULL);



    // Send a bluetooth signal 


}
