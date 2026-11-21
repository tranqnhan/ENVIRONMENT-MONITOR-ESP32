#include "p_ident.h"
#include "p_nvs.h"

#include "esp_err.h"
#include "nvs_flash.h"

static char _username[USERNAME_SIZE] = {0};
static bool _has_username = false;

void ident_load_username(void)
{
    _username[0] = '\0';

    esp_err_t err = nvs_read_string(
        NVS_USERNAME_KEY,
        _username,
        sizeof(_username)
    );

    _has_username = (err == ESP_OK && _username[0] != '\0');
}

bool ident_username_exists(void) {
    return _has_username;
}


const char* ident_get_username(void) {
    return _username;
}
