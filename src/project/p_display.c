#include "freertos/idf_additions.h"
#include "p_screen.h"
#include "string.h"
#include "esp_err.h"
#include "ssd1306.h"

#include "p_i2c.h"
#include "p_display.h"
#include "p_scd41.h"


ssd1306_handle_t oled_dev_hdl;


esp_err_t display_init(void) {
    ssd1306_config_t dev_cfg = I2C_SSD1306_128x32_CONFIG_DEFAULT;
    esp_err_t err = ssd1306_init(*get_i2c_bus_handle(), &dev_cfg, &oled_dev_hdl);
    
    ssd1306_set_contrast(oled_dev_hdl, 0xff);

    return err;
}


static bool _take_screen_semaphore(const enum Screen screen) {
    if (!xSemaphoreTake(xScreenMutex, 0)) {
        return false;
    }

    if (screen_get_current() != screen) {
        xSemaphoreGive(xScreenMutex);
        return false;
    }

    xSemaphoreGive(xScreenMutex);

    return true;
}


void display_measurements(const measurement_t *scd41_m) {
    if (!_take_screen_semaphore(MEASUREMENT_SCR)) return;

    ssd1306_clear_display(oled_dev_hdl, false);

    char co2_s[11];
    char tmp_s[17];
    char hmd_s[17];

    sprintf(co2_s, "CO2 %d", scd41_m->co2);
    sprintf(tmp_s, "TMP %.2f", scd41_m->temperature);
    sprintf(hmd_s, "HMD %.2f", scd41_m->humidity);
    
    ssd1306_display_text(oled_dev_hdl, 0, co2_s, false);
    ssd1306_display_text(oled_dev_hdl, 1, tmp_s, false);
    ssd1306_display_text(oled_dev_hdl, 2, hmd_s, false);
    
}


void display_wifi_provision(const char *username, const char *password, const uint32_t seconds_remaining, const char *message) {
    if (!_take_screen_semaphore(WIFI_PROVISION_SCR)) return;
    
    ssd1306_clear_display(oled_dev_hdl, false);

    const char *username_text = "USR ";
    const char *password_text = "POP ";
    
    char username_disp[strlen(username_text) + strlen(username) + 1];
    char password_disp[strlen(password_text) + strlen(password) + 1];

    sprintf(username_disp, "%s%s", username_text, username);
    sprintf(password_disp, "%s%s", password_text, password);

    const int minutes = seconds_remaining / 60;
    const int seconds = seconds_remaining % 60;

    const char *timer_text = "00:00";
    char timer_disp[strlen(timer_text) + 1];

    sprintf(timer_disp, "%02d:%02d", minutes % 100, seconds % 100);

    ssd1306_display_text(oled_dev_hdl, 0, username_disp, false);
    ssd1306_display_text(oled_dev_hdl, 1, password_disp, false);
    ssd1306_display_text(oled_dev_hdl, 2, timer_disp, false);
    ssd1306_display_text(oled_dev_hdl, 3, message, false);

}


void display_wifi_provision_message(const char *message) {
    if (!_take_screen_semaphore(WIFI_PROVISION_SCR)) return;

    ssd1306_clear_display_page(oled_dev_hdl, 3, false);
    ssd1306_display_text(oled_dev_hdl, 3, message, false);

}


void display_wifi_provision_time(const uint32_t seconds_remaining) {
    if (!_take_screen_semaphore(WIFI_PROVISION_SCR)) return;

    const int minutes = seconds_remaining / 60;
    const int seconds = seconds_remaining % 60;

    const char *timer_text = "00:00";
    char timer_disp[strlen(timer_text) + 1];

    sprintf(timer_disp, "%02d:%02d", minutes % 100, seconds % 100);

    ssd1306_display_text(oled_dev_hdl, 2, timer_disp, false);
}