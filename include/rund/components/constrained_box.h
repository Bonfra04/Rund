#pragma once

#include "component.h"

typedef struct box_constraints
{
    uint64_t min_width;
    uint64_t min_height;
    uint64_t max_width;
    uint64_t max_height;
} box_constraints_t;

typedef struct constrained_box
{
	component_t base;
    box_constraints_t constraints;
	component_t* child;
} __attribute__((packed)) constrained_box_t;

#ifdef __cplusplus
extern "C" {
#endif

#define BoxConstraints(_min_width, _min_height, _max_width, _max_height) (box_constraints_t){ _min_width, _min_height, _max_width, _max_height }

constrained_box_t* constrained_box_create(component_t* child, box_constraints_t constraints);

#ifdef __cplusplus
}
#endif

#define ConstrainedBox(child, constraints) ((component_t*)constrained_box_create(child, constraints))