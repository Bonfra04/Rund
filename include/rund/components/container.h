#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct container_attributes
{
	char id[ID_LEN];
	component_t* child;
	color_t* color;
	size_t* width;
	size_t* height;
} container_attributes_t;

typedef struct container
{
	component_t base;
	container_attributes_t attributes;
} __attribute__((packed)) container_t;

container_t* container_create(container_attributes_t attributes);

#define Container(...) ((component_t*)container_create((container_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
