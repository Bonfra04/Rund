#pragma once

#include <rund/utils/vector.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union color
{
	uint32_t argb;
	struct
	{
		uint8_t a;
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};
} color_t;

#define Color(color) ((color_t){ .argb = (color) })

typedef struct buffer
{
	size_t width, height;
	color_t* data;
} buffer_t;

buffer_t buffer_create(size_t width, size_t height);
void buffer_destroy(buffer_t* buffer);

color_t blend(color_t bg, color_t fg);

typedef struct dimension
{
	size_t width, height;
} dimension_t;

typedef struct coord
{
	uint64_t x, y;
} coord_t;

typedef struct build_context
{
	size_t max_width, max_height;
	size_t min_width, min_height;
	buffer_t backbuffer;
} build_context_t;

typedef struct component component_t;

typedef struct widget_position
{
    dimension_t dimensions;
    coord_t coords;
    uint64_t z;
    component_t* component;
} widget_position_t;

typedef struct draw_data
{
	dimension_t dims;
	coord_t coords;
	vector_t childs;
} draw_data_t;

#define NULL_DRAW ((draw_data_t){ .dims = { 0, 0 }, .coords = { 0, 0 }, .childs = NULL })

#ifdef __cplusplus
}
#endif
