#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

Component(container,
    component_t* child;
    color_t* color;
    size_t* width;
    size_t* height;
);

#define Container(...) ((component_t*)container_create((container_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
