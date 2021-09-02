#pragma once

#include "component.h"
#include <stddef.h>

typedef struct alignment
{
    float x, y;
} alignment_t;

typedef struct align
{
	component_t base;
    alignment_t alignment;
	component_t* child;
} __attribute__((packed)) align_t;

#ifdef __cplusplus
extern "C" {
#endif

#define Alignment(_x, _y) ((alignment_t){ .x = _x, .y = _y })

#define DEF_ALIGN Alignment(0.0f, 0.0f)

align_t* align_create(component_t* child, alignment_t alignment);

#ifdef __cplusplus
}
#endif

#include "../vrg.h"

#define Align1(child) ((component_t*)align_create(child, DEF_ALIGN))
#define Align2(child, alignment) ((component_t*)align_create(child, alignment))

#define Align(...) vrg(Align, __VA_ARGS__)