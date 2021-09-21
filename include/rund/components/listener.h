#pragma once

#include "component.h"
#include <stddef.h>

typedef struct handlers
{
    void (*on_pointer_up)();
    void (*on_pointer_down)();
} handlers_t;

#define Handlers (handlers_t)

typedef struct listener
{
	component_t base;
	component_t* child;
    handlers_t handlers;
} __attribute__((packed)) listener_t;

#ifdef __cplusplus
extern "C" {
#endif

listener_t* listener_create(component_t* child, handlers_t handlers);

#ifdef __cplusplus
}
#endif

#define Listener(child, handlers) ((component_t*)listener_create(child, handlers))

