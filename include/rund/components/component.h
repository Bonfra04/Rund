#pragma once

#include "../core.h"
#include <stdbool.h>

#define ID_LEN 16

typedef enum component_type
{
	EMPTY = 0,
	CONTAINER,
	ROW,
	EXPANDED,
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
	bool isFlexible;
} __attribute__((packed)) component_t;

typedef struct flexible
{
	component_type_t type;
	component_t* parent;
	char id[ID_LEN];
	bool isFlexible;
	uint64_t* flex;
} __attribute__((packed)) flexible_t;

typedef struct component_list
{
	component_t** components;
	size_t length;
} component_list_t;

#define list(...) ((component_list_t){((component_t**)(component_t*[]){__VA_ARGS__}), vrg_argn(__VA_ARGS__)})
