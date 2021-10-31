#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct flexible_attributes
{
	char id[ID_LEN];
	component_t* child;
	uint64_t* flex;
} flexible_attributes_t;

typedef struct flexible
{
	component_t base;
	flexible_attributes_t attributes;
} __attribute__((packed)) flexible_t;

flexible_t* flexible_create(flexible_attributes_t attributes);

#define Flexible(...) ((component_t*)flexible_create((flexible_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
