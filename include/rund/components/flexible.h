#pragma once

#include <rund/component.h>

#ifdef __cplusplus
extern "C" {
#endif

Component(flexible,
	component_t* child;
	uint64_t* flex;
);

#define Flexible(...) ((component_t*)flexible_create((flexible_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif
