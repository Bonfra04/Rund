#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct alignment
{
    float x, y;
} alignment_t;

#define Alignment(_x, _y) (clone(&(alignment_t){ .x = _x > 1.0f ? 1.0f : _x < -1.0f ? -1.0f : _x, .y = _y > 1.0f ? 1.0f : _y < -1.0f ? -1.0f : _y }, sizeof(alignment_t)))

Component(align,
    alignment_t* alignment;
	component_t* child;
);

#define Align(...) ((component_t*)align_create((align_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
