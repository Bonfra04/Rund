#pragma once

#include "rund.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void create_window(const rund_app_t* app);
bool event_loop();
void put_pixel(size_t x, size_t y, uint32_t color);
void flush();

#ifdef __cplusplus
}
#endif
