#pragma once

#include <rund/memory/allocation.h>
#include <rund/vrg.h>
#include <rund/rendering.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ID_LEN 16

#define FLAG_NONE       (0 << 0)
#define FLAG_FLEXIBLE   (1 << 0)
#define FLAG_LISTENER   (1 << 1)

typedef struct component component_t;

typedef draw_data_t (*draw_func_t)(const component_t* component, const build_context_t context, uint64_t deepness);
#define DrawFunc(func) ((draw_func_t)func)

draw_data_t draw_component(const component_t* component, const component_t* parent, const build_context_t context, uint64_t deepness);

typedef struct component
{
    component_t* parent;
    draw_func_t draw_func;
	uint8_t flags;
    char id[ID_LEN];
} __attribute__((packed)) component_t;

typedef struct component_list
{
    component_t** components;
    size_t length;
} component_list_t;

#define list(...) (clone(&(component_list_t){((component_t**)(component_t*[]){__VA_ARGS__}), vrg_argn(__VA_ARGS__)}, sizeof(component_list_t)))

#define Component(name, ...)        \
typedef struct name##_attributes    \
{                                   \
    char id[ID_LEN];                \
    __VA_ARGS__                     \
} name##_attributes_t;              \
typedef struct name                 \
{                                   \
    component_t base;               \
    __VA_ARGS__                     \
} __attribute__((packed)) name##_t;	\
name##_t* name##_create(name##_attributes_t attributes)

#ifdef __cplusplus
}
#endif
