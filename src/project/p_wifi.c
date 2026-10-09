#include <stdint.h>
#include <stdio.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_srp.h"

#include "esp_wifi_types_generic.h"
#include "network_provisioning/manager.h"
#include "network_provisioning/scheme_ble.h"
#include "esp_random.h"

#include "p_wifi.h"



#define SALT_LEN 16

static const char *USERNAME_HEADER = "EMS_";

static const char *TAG = "WIFI";

static void wifi_register_events();

void wifi_init() 
{
    // Network stack
    ESP_ERROR_CHECK(esp_netif_init()); 
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // Wifi configurations
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    // Register events
    wifi_register_events();



}


static void generate_password(char *out, size_t out_len)
{
    static const char alphabet[] =
        "abcdefghijkmnopqrstuvwxyz"
        "0123456789";


    for (size_t i = 0; i < out_len - 1; i++) {
        uint32_t r = esp_random();
        out[i] = alphabet[r % (sizeof(alphabet) - 1)];
    }

    out[out_len - 1] = '\0';
}


credential_ble_t wifi_start_provision() 
{
    // Network provisioning configurations
    network_prov_mgr_config_t provision_config = {
        .scheme = network_prov_scheme_ble,
        .scheme_event_handler = NETWORK_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
    };
    ESP_ERROR_CHECK(network_prov_mgr_init(provision_config));

    // Generate salt and verifier
    char *salt = NULL;
    char *verifier = NULL;
    int verifier_len = 0;

    char username[USERNAME_LEN + 1];
    char usernameTail[(USERNAME_LEN - strlen(USERNAME_HEADER)) + 1];
    generate_password(usernameTail, sizeof(usernameTail));
    snprintf(username, sizeof(username), "%s%s", USERNAME_HEADER, usernameTail);

    char password[PASSWORD_LEN + 1];
    generate_password(password, sizeof(password));

    ESP_LOGI(TAG, "username %s password %s", username, password);

    ESP_ERROR_CHECK(esp_srp_gen_salt_verifier(
        username,
        strlen(username),
        password,
        strlen(password),
        &salt,
        SALT_LEN,                
        &verifier,
        &verifier_len
    ));

    network_prov_security2_params_t sec2_params = {
        .salt = salt,
        .salt_len = SALT_LEN,
        .verifier = verifier,
        .verifier_len = verifier_len
    };
    
    // Start provisioning
    ESP_ERROR_CHECK(
        network_prov_mgr_start_provisioning(
            NETWORK_PROV_SECURITY_2,
            &sec2_params,
            username,
            NULL
        )
    );

    ESP_LOGI(TAG, "BLE provisioning start");

    credential_ble_t cred_ble;
    snprintf(cred_ble.username, sizeof(cred_ble.username),"%s", username);
    snprintf(cred_ble.password, sizeof(cred_ble.password),"%s", password);

    return cred_ble;
}


void wifi_start_station() 
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}


static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) 
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "Wi-Fi disconnected");
                break;
            default:
                break;
        }
        return;
    }

    if (event_base == NETWORK_PROV_EVENT) {
        switch (event_id) {
            case NETWORK_PROV_START:
                ESP_LOGI(TAG, "Start provisioning");
                break;
            case NETWORK_PROV_WIFI_CRED_RECV:
                wifi_sta_config_t *wifi_config = (wifi_sta_config_t *)event_data;
                ESP_LOGI(TAG, "Received wifi credentials %s", (char *) wifi_config->ssid);
                break;
            case NETWORK_PROV_WIFI_CRED_SUCCESS:
                ESP_LOGI(TAG, "Provisioning successful");
                break;

            case NETWORK_PROV_WIFI_CRED_FAIL:
                ESP_LOGE(TAG, "Provisioning failed");
                network_prov_mgr_reset_wifi_sm_state_on_failure();
                break;
            default:
                break;
        }
        return;
    }

    if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP:
                ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
                ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
                break;
            default:
                break;
        }
        return;
    }
}

static void wifi_register_events() 
{
    ESP_ERROR_CHECK(
        esp_event_handler_register(
            NETWORK_PROV_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL
        )
    );

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL
        )
    );

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL
        )
    );
}



