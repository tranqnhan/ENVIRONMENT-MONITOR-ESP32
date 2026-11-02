#pragma once

#include "button_types.h"
#include <stdbool.h>
#include <stdint.h>

#define LONG_PRESS_TIME 3000

button_handle_t register_gpio_button(uint16_t gpio_num);
void regiter_gpio_button_callback(button_handle_t gpio_btn, void (*callback)(void *arg,void *usr_data));