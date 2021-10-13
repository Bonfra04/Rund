#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct expanded_attributes
{
	char id[ID_LEN];
	component_t* child;
	uint64_t* flex;
} expanded_attributes_t;

typedef struct expanded
{
	flexible_t base;
	expanded_attributes_t attributes;
} __attribute__((packed)) expanded_t;

expanded_t* expanded_create(expanded_attributes_t attributes);

#define Expanded(...) ((component_t*)expanded_create((expanded_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
