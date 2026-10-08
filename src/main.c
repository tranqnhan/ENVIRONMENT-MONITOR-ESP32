#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"

#include "p_i2c.h"
#include "p_nvs.h"
#include "p_scd41.h"
#include "p_display.h"
#include "p_log.h"
#include "p_tasks.h"


static void setup() {

    esp_err_t err;
    
    err = i2c_init();
    log_command(err, "i2c init");

    err = display_init();
    log_command(err, "display init");

    err = scd41_init();
    log_command(err, "scd41 init");

    err = scd41_start_periodic_measurement();
    log_command(err, "scd41 start periodic measurement");

    nvs_init();
}




void app_main(void)
{
    // Give USB Serial/JTAG time to connect
    vTaskDelay(pdMS_TO_TICKS(1000));

    setup();

    xTaskCreate(&environment_monitor_task_measure, "Environment Monitor", 1500, NULL, 5, NULL);

}

