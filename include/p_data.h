#pragma once

#include <stddef.h>

#define PASSWORD_LEN 8
#define USERNAME_LEN 8

void data_init();
void data_get_username(char *username, size_t size);
void data_set_username(const char *username, size_t size);
