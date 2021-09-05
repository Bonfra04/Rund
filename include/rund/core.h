#pragma once

#include <stdint.h>
#include <stddef.h>
#include <rund/utils/vector.h>
#include <rund/vrg.h>

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
	dimension_t dimensions;
	coord_t coords;
	vector_t childs;
} draw_data_t;

#define NULL_DRAW ((draw_data_t){ .dimensions = { 0, 0 }, .coords = { 0, 0 }, .childs = NULL })

typedef enum axis_alignment
{
	axis_start,
	axis_end,
	axis_center,
} axis_alignment_t;

#define list(...) ((component_t**)(component_t*[]){__VA_ARGS__}), vrg_argn(__VA_ARGS__)

buffer_t buffer_create(size_t width, size_t height);
void buffer_destroy(buffer_t* buffer);
