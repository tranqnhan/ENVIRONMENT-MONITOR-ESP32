#pragma once

enum Screen {
    MEASUREMENT,
    WIFI_PROVISION,
    NUM_SCREEN
};

typedef struct {
    void (*onExit)(void);
    void (*onEnter)(void);
} screen_t;


screen_t screens[NUM_SCREEN];

void screen_mgr_init();

void screen_wifi_provision_on_enter();
void screen_wifi_provision_on_exit();
void screen_measurement_on_enter();
void screen_measurement_on_exit();
void screen_change_to(enum Screen screen);

