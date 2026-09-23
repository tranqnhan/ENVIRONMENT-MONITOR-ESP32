#include "esp_err.h"

#include "ssd1306.h"

#include "p_i2c.h"
#include "p_display.h"
#include "p_scd41.h"


ssd1306_handle_t oled_dev_hdl;

esp_err_t display_init(void) {
    ssd1306_config_t dev_cfg = I2C_SSD1306_128x32_CONFIG_DEFAULT;
    esp_err_t err = ssd1306_init(*get_i2c_bus_handle(), &dev_cfg, &oled_dev_hdl);
    
    return err;
}


void display_measurements(const measurement_t *scd41_m) {
    ssd1306_clear_display(oled_dev_hdl, false);
    ssd1306_set_contrast(oled_dev_hdl, 0xff);
    
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
