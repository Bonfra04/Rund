#pragma once

#include "component.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct container_decoration
{
	color_t color;
} container_decoration_t;

#define ContainerDec(...) ((container_decoration_t){__VA_ARGS__})

typedef struct container_attributes
{
	char id[ID_LEN];
	container_decoration_t* decoration;
	component_t* child;
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
