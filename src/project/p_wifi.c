#include <stdint.h>
#include <string.h>

#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"
#include "esp_log.h"

#include "p_log.h"
#include "p_wifi.h"


#define ESP_WIFI_SSID "esp_tutorial"
#define ESP_WIFI_PASS "trouble123"
#define ESP_WIFI_CHANNEL 1
#define MAX_STA_CONN 2


uint16_t is_wifi_softap_init = 0;



static void wifi_event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGI(DEVICE_NAME, "Event nr: %ld!\n", event_id);
}

void wifi_softap_init() 
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();
    
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&wifi_init_config);

    esp_event_handler_instance_register(
        WIFI_EVENT, 
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        NULL
    );


    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = true,
            }
        },

    };

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();

    is_wifi_softap_init = 1;
}

void wifi_softap_deinit() {
    if (is_wifi_softap_init) {
    esp_wifi_stop();
    }
}

#include "network_provisioning/network_scan.h"
