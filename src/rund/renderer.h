#pragma once

#include <rund/core.h>

void renderer_init();

void plot_pixel(const buffer_t* buffer, uint64_t x, uint64_t y, const color_t color);
void draw_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, color_t color, const buffer_t* buffer);
void draw_triangle(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, color_t color, const buffer_t* buffer);
void fill_polygon(uint64_t* xCoords, uint64_t* yCoords, size_t nPoints, color_t color, const buffer_t* buffer);
draw_data_t draw_character(const buffer_t* buffer, color_t color, char character, uint64_t transX, uint64_t transY, size_t scale);

color_t blend(color_t bg, color_t fg);
#define BLEND(bg, fg) {bg = blend(bg, fg);}