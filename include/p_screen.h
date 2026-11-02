#pragma once

#include "freertos/idf_additions.h"

enum Screen {
    MEASUREMENT_SCR,
    WIFI_PROVISION_SCR,
    NUM_SCREEN
};

typedef struct {
    void (*on_exit)(void);
    void (*on_enter)(void);
} screen_t;

extern SemaphoreHandle_t xScreenMutex;

void screen_mgr_init(void);
void screen_change_to(const enum Screen screen);
enum Screen screen_get_current();
bool screen_semaphore_check(const enum Screen screen);

void screen_wifi_provision_on_enter(void);
void screen_wifi_provision_on_exit(void);
void screen_measurement_on_enter(void);
void screen_measurement_on_exit(void);
