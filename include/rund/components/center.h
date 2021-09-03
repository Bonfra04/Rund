#pragma once

#include "component.h"
#include <rund/components/align.h>
#include <stddef.h>

typedef struct center
{
	component_t base;
	align_t align;
} __attribute__((packed)) center_t;

#ifdef __cplusplus
extern "C" {
#endif

center_t* center_create(component_t* child);

#ifdef __cplusplus
}
#endif

#include "../vrg.h"

#define Center1(child) ((component_t*)center_create(child))

#define Center(...) vrg(Align, __VA_ARGS__)