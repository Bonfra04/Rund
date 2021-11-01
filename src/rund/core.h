#pragma once

#include <rund.h>

#include <rund/utils/vector.h>
#include <rund/memory/gc.h>

typedef uint32_t color_t;

typedef struct dimension
{
	size_t width, height;
} dimension_t;

typedef struct coord
{
	uint64_t x, y;
} coord_t;

typedef struct buffer
{
	size_t width, height;
	color_t* data;
} buffer_t;

typedef struct build_context
{
	size_t max_width, max_height;
	size_t min_width, min_height;
	buffer_t backbuffer;
} build_context_t;

typedef struct draw_data
{
	dimension_t dims;
	coord_t coords;
	vector_t childs;
} draw_data_t;

typedef struct widget_position
{
    dimension_t dimensions;
    coord_t coords;
    uint64_t z;
    component_t* component;
} widget_position_t;

#define NULL_DRAW ((draw_data_t){ .dims = { 0, 0 }, .coords = { 0, 0 }, .childs = NULL })

buffer_t buffer_create(size_t width, size_t height);
void buffer_destroy(buffer_t* buffer);
