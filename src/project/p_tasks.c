#include "freertos/FreeRTOS.h"
#include "network_provisioning/manager.h"

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

    vTaskDelete(NULL);
}


#define MAX_PROVISION_TIME_SECS 300
static int provision_time_remaining = 0;

void environment_monitor_task_wifi_provision(void *pvParameter) {
    provision_time_remaining = MAX_PROVISION_TIME_SECS;

    wifi_init();
    
    // Wifi provision status
    bool is_wifi_provisioned = false;
    ESP_ERROR_CHECK(network_prov_mgr_is_wifi_provisioned(&is_wifi_provisioned));

    if (is_wifi_provisioned) {
        wifi_start_station();
    } else {
        credential_ble_t cred_ble = wifi_start_provision();

        display_wifi_provision(cred_ble.username, cred_ble.password, provision_time_remaining, "BLE WIFI PROV");

        while (1) {
            provision_time_remaining -= 1;

            display_wifi_provision_time(provision_time_remaining);

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }


    vTaskDelete(NULL);
}