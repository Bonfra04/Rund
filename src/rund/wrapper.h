#pragma once

#include <rund.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct events {
    void (*mouse_down)(uint64_t x, uint64_t y, uint8_t button);
    void (*mouse_up)(uint64_t x, uint64_t y, uint8_t button);
    void (*key_down)(uint16_t key);
    void (*key_up)(uint16_t key);
    void (*resize)(uint64_t width, uint64_t height);
} events_t;

void create_window(const rund_app_t* app, events_t handlers);
bool event_loop();
void put_pixel(size_t x, size_t y, uint32_t color);
void flush();

#ifdef __cplusplus
}
#endif
