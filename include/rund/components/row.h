#pragma once

#include "component.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct row_decoration
{
	color_t color;
} row_decoration_t;

typedef struct row_attributes
{
	char id[ID_LEN];
	row_decoration_t* decoration;
	component_list_t* children;
} row_attributes_t;

typedef struct row
{
	component_t base;
	row_attributes_t attributes;
} __attribute__((packed)) row_t;

row_t* row_create(row_attributes_t attributes);

#define Row(...)((component_t*)row_create((row_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
