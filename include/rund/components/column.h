#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct column_decoration
{
	color_t color;
} column_decoration_t;

typedef struct column_attributes
{
	char id[ID_LEN];
	column_decoration_t* decoration;
	component_list_t* children;
} column_attributes_t;

typedef struct column
{
	component_t base;
	column_attributes_t attributes;
} __attribute__((packed)) column_t;

column_t* column_create(column_attributes_t attributes);

#define Column(...)((component_t*)column_create((column_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
