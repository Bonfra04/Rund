#pragma once

#include <stdint.h>
#include <stddef.h>
#include "vrg.h"

typedef uint32_t color_t;

typedef struct build_context
{
	size_t max_width, max_height;
	size_t min_width, min_height;
	color_t* backbuffer;
} build_context_t;

typedef struct dimension
{
	size_t width, height;
} dimension_t;

typedef struct coord
{
	uint64_t x, y;
} coord_t;

typedef struct draw_data
{
	dimension_t dimensions;
	coord_t coords;
} draw_data_t;

#define NULL_DRAW ((draw_data_t){ { 0, 0 }, { 0, 0 } })

typedef enum axis_alignment
{
	axis_start,
	axis_end,
	axis_center,
} axis_alignment_t;

#define list(...) ((component_t**)(component_t*[]){__VA_ARGS__}), vrg_argn(__VA_ARGS__)
