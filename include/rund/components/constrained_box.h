#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct box_constraints
{
    uint64_t min_width;
    uint64_t min_height;
    uint64_t max_width;
    uint64_t max_height;
} box_constraints_t;

#define BoxConstraints(_min_width, _min_height, _max_width, _max_height) (clone(&(box_constraints_t){ _min_width, _min_height, _max_width, _max_height }, sizeof(box_constraints_t)))

typedef struct constrained_box_attributes
{
    char id[ID_LEN];
    box_constraints_t* constraints;
	component_t* child;
} constrained_box_attributes_t;

typedef struct constrained_box
{
	component_t base;
    constrained_box_attributes_t attributes;
} __attribute__((packed)) constrained_box_t;

constrained_box_t* constrained_box_create(constrained_box_attributes_t attributes);

#define ConstrainedBox(...) ((component_t*)constrained_box_create((constrained_box_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
