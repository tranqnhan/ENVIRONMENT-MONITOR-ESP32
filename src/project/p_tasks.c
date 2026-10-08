#include "freertos/FreeRTOS.h"

#include "p_tasks.h"

#include "p_wifi.h"
#include "p_scd41.h"
#include "p_display.h"


void environment_monitor_task_measure(void *pvParameter) {
    bool is_data_ready;
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


void environment_monitor_task_wifi_provision(void *pvParameter) {

}