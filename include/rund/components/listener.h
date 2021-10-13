#pragma once

#include "component.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct handlers
{
    void (*on_pointer_up)(component_t* self);
    void (*on_pointer_down)(component_t* self);
} handlers_t;

#define Handlers(...) ((handlers_t*)clone(&(handlers_t){__VA_ARGS__}, sizeof(handlers_t)))

typedef struct listener_attributes
{
    char id[ID_LEN];
    component_t* child;
    handlers_t* handlers;
} listener_attributes_t;

typedef struct listener
{
	component_t base;
	listener_attributes_t attributes;
} __attribute__((packed)) listener_t;

listener_t* listener_create(listener_attributes_t attributes);

#define Listener(...) ((component_t*)listener_create((listener_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
