#pragma once

#include <rund/memory/allocation.h>
#include <rund/vrg.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ID_LEN 16

typedef uint32_t color_t;

typedef enum component_type
{
    EMPTY = 0,
    CONTAINER,
    LAYOUT,
    FLEXIBLE,
    ALIGN,
    CONSTRAINED_BOX,
    LISTENER,
    TEXT
} component_type_t;

typedef struct component component_t;

typedef struct component
{
    component_type_t type;
    component_t* parent;
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
