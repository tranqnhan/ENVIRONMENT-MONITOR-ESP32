#include "p_button.h"

#include "button_gpio.h"
#include "iot_button.h"
#include <stdint.h>

#define LONG_PRESS_TIME 2000

button_handle_t register_gpio_button(uint16_t gpio_num) {
    const button_config_t btn_cfg = {0};
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = gpio_num,
        .active_level = 0,
    };
    button_handle_t gpio_btn = NULL;
    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, &gpio_btn);
    
    ESP_ERROR_CHECK(ret);

    return gpio_btn;
}


void regiter_gpio_button_callback(button_handle_t gpio_btn, void (*callback)(void *arg,void *usr_data)) {
    button_event_args_t args = {
        .long_press.press_time = LONG_PRESS_TIME,
    };

    iot_button_register_cb(gpio_btn, BUTTON_LONG_PRESS_START, &args, callback, NULL);

}