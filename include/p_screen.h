#pragma once

typedef struct {
    void (*onExit)(void);
    void (*onEnter)(void);
} screen_t;