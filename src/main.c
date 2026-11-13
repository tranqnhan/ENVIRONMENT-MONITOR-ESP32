#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"

#include "p_i2c.h"
#include "p_nvs.h"
#include "p_scd41.h"
#include "p_display.h"
#include "p_screen.h"
#include "p_wifi.h"

static void setup() 
{
    
    ESP_ERROR_CHECK(i2c_init());
    ESP_ERROR_CHECK(display_init());
    ESP_ERROR_CHECK(scd41_init());
    scd41_start_periodic_measurement();

    nvs_init();
    wifi_init();
    screen_mgr_init();
}


void app_main(void)
{
    // Give USB Serial/JTAG time to connect
    vTaskDelay(pdMS_TO_TICKS(1000));

    setup();
}

