#pragma once

#include "component.h"
#include <stddef.h>

typedef struct expanded
{
	flexible_t base;
	component_t* child;
} __attribute__((packed)) expanded_t;

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_FLEX 1

expanded_t* expanded_create(component_t* child, uint64_t flex);

#ifdef __cplusplus
}
#endif

#include "../vrg.h"

#define Expanded1(child) ((component_t*)expanded_create(child, DEF_FLEX))
#define Expanded2(child, flex) ((component_t*)expanded_create(child, flex))

#define Expanded(...) vrg(Expanded, __VA_ARGS__)