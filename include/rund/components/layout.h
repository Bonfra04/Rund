#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum laying_style
{
    Stack, Column, Row
} laying_style_t;

typedef struct layout_attributes
{
    char id[ID_LEN];
    component_list_t* children;
    laying_style_t* laying_style;
} layout_attributes_t;

typedef struct layout
{
	component_t base;
    layout_attributes_t attributes;
} __attribute__((packed)) layout_t;

layout_t* layout_create(layout_attributes_t attributes);

#define Layout(...) ((component_t*)layout_create((layout_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif