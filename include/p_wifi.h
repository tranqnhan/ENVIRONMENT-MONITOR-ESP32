#pragma once

#include <stdbool.h>

#include "p_nvs.h"

#define SALT_LEN 16
#define MAX_WIFI_START_CB 2

typedef struct {
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} credential_ble_t;

void wifi_init(void);
credential_ble_t wifi_start_provision(void);
void wifi_stop_provision(void);
bool wifi_is_provision(void);
bool wifi_is_connected(void);
esp_err_t wifi_erase_credentials(void);
void wifi_start_station(void);

void wifi_start_register_callback(void (*on_wifi_start_cb)(void));

