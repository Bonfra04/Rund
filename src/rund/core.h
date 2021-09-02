#pragma once

#include <stdint.h>
#include <stddef.h>
#include "vrg.h"

typedef uint32_t color_t;

typedef struct build_context
{
	size_t width, height;
	uint64_t x, y;
	color_t* backbuffer;
} build_context_t;

typedef struct dimension
{
	size_t width, height;
} dimension_t;

typedef enum axis_alignment
{
	axis_start,
	axis_end,
	axis_center,
} axis_alignment_t;

#define list(...) ((component_t**)(component_t*[]){__VA_ARGS__}), vrg_argn(__VA_ARGS__)
