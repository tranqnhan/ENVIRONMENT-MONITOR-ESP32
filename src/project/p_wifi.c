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
#include "p_http_client.h"
#include "p_nvs.h"
#include "p_time.h"

static const char *USERNAME_HEADER = "EMS_";
static const char *TAG = "WIFI";
static bool _is_wifi_connected = false;

static void _wifi_register_events(void);
static void _generate_random_str(char *out, size_t out_len);
static void _init_http_and_ntp(void);

static uint16_t num_on_wifi_start_cbs = 0;
static void (*on_wifi_start_cbs[MAX_WIFI_START_CB])(void);


void wifi_init(void) 
{
    _is_wifi_connected = false;

    // Network stack
    ESP_ERROR_CHECK(esp_netif_init()); 
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // Wifi configurations
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    wifi_start_register_callback(_init_http_and_ntp);

    // Register events
    _wifi_register_events();
}

void wifi_start_register_callback(void (*on_wifi_start_cb)(void)) {
    if (num_on_wifi_start_cbs < MAX_WIFI_START_CB) {
        on_wifi_start_cbs[num_on_wifi_start_cbs] = on_wifi_start_cb;
        num_on_wifi_start_cbs++;
    }
}


bool wifi_is_provision(void) {
    bool is_wifi_provisioned = false;
    ESP_ERROR_CHECK(network_prov_mgr_is_wifi_provisioned(&is_wifi_provisioned));
    return is_wifi_provisioned;
}


bool wifi_is_connected(void) {
    return _is_wifi_connected;
}


credential_ble_t wifi_start_provision(void) 
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
    _generate_random_str(usernameTail, sizeof(usernameTail));
    snprintf(username, sizeof(username), "%s%s", USERNAME_HEADER, usernameTail);

    char password[PASSWORD_LEN + 1];
    _generate_random_str(password, sizeof(password));

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

    nvs_save_string(NVS_USERNAME_KEY, username);
    nvs_save_string(NVS_PASSWORD_KEY, password);

    // TODO: add database url

    return cred_ble;
}


esp_err_t wifi_erase_credentials(void)
{
    wifi_config_t wifi_config = {0};

    return esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
}


void wifi_stop_provision(void) 
{
    network_prov_mgr_deinit();
}


void wifi_start_station(void) 
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}


static void _generate_random_str(char *out, size_t out_len)
{
    static const char alphabet[] =
        "abcdefghijkmnopqrstuvwxyz"
        "0123456789"
        "!@#$^&*"
    ;


    for (size_t i = 0; i < out_len - 1; i++) {
        uint32_t r = esp_random();
        out[i] = alphabet[r % (sizeof(alphabet) - 1)];
    }

    out[out_len - 1] = '\0';
}

static void _wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) 
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();

                break;
            case WIFI_EVENT_STA_CONNECTED:
                _is_wifi_connected = true;
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
                // wifi_sta_config_t *wifi_config = (wifi_sta_config_t *)event_data;
                ESP_LOGI(TAG, "Received wifi credentials");
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

static void _wifi_register_events(void) 
{
    ESP_ERROR_CHECK(
        esp_event_handler_register(
            NETWORK_PROV_EVENT,
            ESP_EVENT_ANY_ID,
            &_wifi_event_handler,
            NULL
        )
    );

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &_wifi_event_handler,
            NULL
        )
    );

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &_wifi_event_handler,
            NULL
        )
    );
}

static void _init_http_and_ntp(void) {
    char server_url[SERVER_URL_LEN + 1];
    nvs_read_string(NVS_SERVER_URL, server_url, sizeof(server_url));
    http_client_init(server_url);
    time_sntp_init();
}

