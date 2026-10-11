#include "freertos/idf_additions.h"
#include "p_wifi.h"
#include "portmacro.h"
#include "button_types.h"
#include "soc/gpio_num.h"

#include "p_button.h"
#include "p_screen.h"
#include "p_tasks.h"

#define INCLUDE_vTaskSuspend 1

static enum Screen current_screen;
static screen_t screens[NUM_SCREEN];
static TaskHandle_t xMeasurementTask = NULL;
static TaskHandle_t xWifiProvisionTask = NULL;

SemaphoreHandle_t xScreenMutex;

static void _on_button_change_screen(void *arg,void *usr_data) {
    enum Screen next_screen = NUM_SCREEN;

    if (xSemaphoreTake(xScreenMutex, portMAX_DELAY)) {
        next_screen = current_screen + 1;
        if (next_screen >= NUM_SCREEN) {
            next_screen = 0;
        }
        xSemaphoreGive(xScreenMutex);
    }

    if (next_screen != NUM_SCREEN) {
        screen_change_to(next_screen);
    }
}


void screen_mgr_init() {
    xScreenMutex = xSemaphoreCreateMutex();
    screens[MEASUREMENT_SCR].on_enter = screen_measurement_on_enter;
    screens[MEASUREMENT_SCR].on_exit = screen_measurement_on_exit;
    screens[WIFI_PROVISION_SCR].on_enter = screen_wifi_provision_on_enter;
    screens[WIFI_PROVISION_SCR].on_exit = screen_wifi_provision_on_exit;
    

    button_handle_t gpio_button = register_gpio_button(GPIO_NUM_6);
    regiter_gpio_button_callback(gpio_button, _on_button_change_screen); 


    current_screen = MEASUREMENT_SCR;
    screen_measurement_on_enter();

}


void screen_change_to(enum Screen screen) {
    if (xSemaphoreTake(xScreenMutex, portMAX_DELAY)) {
        if (current_screen == screen || screen >= NUM_SCREEN) {
            xSemaphoreGive(xScreenMutex);
            return;
        }

        screens[current_screen].on_exit();
        current_screen = screen;
        screens[current_screen].on_enter();

        xSemaphoreGive(xScreenMutex);
    }
}


enum Screen screen_get_current() {
    return current_screen;
}


void screen_measurement_on_enter() {
    if (xMeasurementTask == NULL) {
        xTaskCreate(&environment_monitor_task_measure, "Measurement", 1500, NULL, 5, &xMeasurementTask);
    }
}


void screen_measurement_on_exit() {

}



void screen_wifi_provision_on_enter() {
    if (xWifiProvisionTask == NULL) {
        xTaskCreate(&environment_monitor_task_wifi_provision, "Wi-Fi Provision", 2048, NULL, 5, &xWifiProvisionTask);
    }
}


void screen_wifi_provision_on_exit() {
    wifi_stop_provision();
    vTaskDelete(xWifiProvisionTask);
    xWifiProvisionTask = NULL;
}