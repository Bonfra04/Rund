#pragma once

#include <rund/rendering.h>

void renderer_init();

void plot_pixel(const buffer_t* buffer, color_t color, uint64_t x, uint64_t y);
void draw_line(const buffer_t* buffer, color_t color, uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1);
draw_data_t draw_character(const buffer_t* buffer, color_t color, wchar_t character, uint64_t transX, uint64_t transY, float scale);
