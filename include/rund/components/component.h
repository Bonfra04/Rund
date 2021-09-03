#pragma once

#include "../core.h"
#include <stdbool.h>

typedef enum component_type
{
	EMPTY = 0,
	CONTAINER,
	ROW,
	EXPANDED,
	ALIGN
} component_type_t;

typedef struct component
{
	component_type_t type;
	bool isFlexible;
} __attribute__((packed)) component_t;

typedef struct flexible
{
	component_type_t type;
	bool isFlexible;
	uint64_t flex;
} __attribute__((packed)) flexible_t;

// useless, just to make code more readable
#define Flex(x) x
