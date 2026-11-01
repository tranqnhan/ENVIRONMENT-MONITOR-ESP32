#include <stddef.h>
#include <stdio.h>

#include "p_data.h"
#include "p_nvs.h"

static char USERNAME[USERNAME_LEN + 1];

void data_init() {
    nvs_read_string(NVS_USERNAME_KEY, USERNAME, USERNAME_LEN + 1);
}


void data_get_username(char *username, size_t size) {
    snprintf(username, size, "%s", USERNAME);
};


void data_set_username(const char *username, size_t size) {
    if (USERNAME_LEN + 1 < size) {
        snprintf(USERNAME, USERNAME_LEN + 1, "%s", username);
    } else {
        snprintf(USERNAME, size, "%s", username);
    }
}