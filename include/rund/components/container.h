#pragma once

#include "component.h"
#include <stddef.h>

typedef struct container_decoration
{
	color_t color;
} container_decoration_t;

#define ContainerDec (container_decoration_t)

typedef struct container
{
	component_t base;
	container_decoration_t decoration;
	component_t* child;
	size_t width, height;
} __attribute__((packed)) container_t;

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_DIM 0
extern container_decoration_t def_dec_contaner;

container_t* container_create(component_t* child, size_t width, size_t height, container_decoration_t decoration);

#ifdef __cplusplus
}
#endif

#include "../vrg.h"

#define Container1(child) ((component_t*)container_create(child, DEF_DIM, DEF_DIM, def_dec_contaner))
#define Container2(child, decoration) ((component_t*)container_create(child, DEF_DIM, DEF_DIM, decoration))
#define Container3(child, width, height) ((component_t*)container_create(child, width, height, def_dec_contaner))
#define Container4(child, width, height, decoration) ((component_t*)container_create(child, width, height, decoration))

#define Container(...) vrg(Container, __VA_ARGS__)
