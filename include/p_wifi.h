#pragma once

#define PASSWORD_LEN 8
#define USERNAME_LEN 8

typedef struct {
    char username[USERNAME_LEN + 1];
    char password[PASSWORD_LEN + 1];
} credential_ble_t;

void wifi_init();
credential_ble_t wifi_start_provision();
void wifi_stop_provision();
void wifi_start_station();