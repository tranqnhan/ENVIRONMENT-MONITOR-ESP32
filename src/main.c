#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"

#include "p_i2c.h"
#include "p_scd41.h"
#include "p_display.h"
#include "p_log.h"
#include "p_wifi.h"


static void setup() {
    esp_err_t err = i2c_init();
    log_command(err, "i2c init");

    err = display_init();
    log_command(err, "display init");

    err = scd41_init();
    log_command(err, "scd41 init");

    err = scd41_start_periodic_measurement();
    log_command(err, "scd41 start periodic measurement");

    wifi_softap_init();
}


static void environment_monitor_task(void *pvParameter) {
    uint32_t is_data_ready;
    measurement_t scd41_m;

    while (1) {
        is_data_ready = 0;
        scd41_data_ready(&is_data_ready);

        if (is_data_ready) {
            scd41_read_measurements(&scd41_m);
            display_measurements(&scd41_m);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }

}


void app_main(void)
{
    // Give USB Serial/JTAG time to connect
    vTaskDelay(pdMS_TO_TICKS(1000));

    setup();

    xTaskCreate(&environment_monitor_task, "Environment Monitor", 1500, NULL, 5, NULL);

}
