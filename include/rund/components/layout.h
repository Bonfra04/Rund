#pragma once

#include <rund/component.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum laying_style
{
    Stack, Column, Row
} laying_style_t;

Component(layout,
    component_list_t* children;
    laying_style_t* laying_style;
);

#define Layout(...) ((component_t*)layout_create((layout_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif