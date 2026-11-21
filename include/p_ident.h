#pragma once

#include "stdbool.h"

void ident_load_username(void);
bool ident_username_exists(void);
const char* ident_get_username(void);