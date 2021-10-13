#pragma once

#include "component.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct alignment
{
    float x, y;
} alignment_t;

#define Alignment(_x, _y) (clone(&(alignment_t){ .x = _x > 1.0f ? 1.0f : _x < -1.0f ? -1.0f : _x, .y = _y > 1.0f ? 1.0f : _y < -1.0f ? -1.0f : _y }, sizeof(alignment_t)))

typedef struct align_attributes
{
    char id[ID_LEN];
    alignment_t* alignment;
	component_t* child;
} align_attributes_t;

typedef struct align
{
	component_t base;
    align_attributes_t attributes;
} __attribute__((packed)) align_t;

align_t* align_create(align_attributes_t attributes);

#define Align(...) ((component_t*)align_create((align_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
