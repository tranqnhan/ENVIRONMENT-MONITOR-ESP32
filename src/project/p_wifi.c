#include <stdint.h>
#include <string.h>

#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"
#include "esp_log.h"

#include "p_log.h"
#include "p_wifi.h"


#define ESP_WIFI_SSID "esp123123"
#define ESP_WIFI_PASS "abcdefg12345"
#define ESP_WIFI_CHANNEL 1
#define MAX_STA_CONN 2


static bool is_wifi_softap_init = false;
static esp_event_handler_instance_t wifi_event_instance;

static void wifi_event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGI(DEVICE_NAME, "event: %ld\n", event_id);
}

void wifi_softap_init() 
{
    esp_err_t err;

    err = esp_netif_init();
    log_command(err, "netif_init");
    
    err = esp_event_loop_create_default();
    log_command(err, "esp_event_loop_create");

    esp_netif_create_default_wifi_ap();
    
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

    err = esp_wifi_init(&wifi_init_config);
    log_command(err, "wifi_init");

    err = esp_event_handler_instance_register(
        WIFI_EVENT, 
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        &wifi_event_instance // Keep the handle for deinit
    );
    log_command(err, "event_handler_instance_register");


    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA3_PSK,
            .pmf_cfg = {
                .required = true,
            }
        },

    };

    err = esp_wifi_set_mode(WIFI_MODE_AP);
    log_command(err, "esp_wifi_set_mode(WIFI_MODE_AP)");

    err = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    log_command(err, "esp_wifi_set_config(WIFI_IF_AP, &wifi_config)");

    err = esp_wifi_start();
    log_command(err, "esp_wifi_start");

    is_wifi_softap_init = true;
}

void wifi_softap_deinit() {
    if (is_wifi_softap_init) {
        ESP_ERROR_CHECK(
            esp_event_handler_instance_unregister(
                WIFI_EVENT,
                ESP_EVENT_ANY_ID,
                wifi_event_instance
            )
        );

        ESP_ERROR_CHECK(esp_wifi_stop());
        ESP_ERROR_CHECK(esp_wifi_deinit());

        is_wifi_softap_init = false;
    }
}

