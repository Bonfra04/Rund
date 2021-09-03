#pragma once

#include <stdint.h>
#include <stddef.h>
#include <rund/components.h>
#include <rund/core.h>

typedef struct rund_app
{
	char title[50];
	uint64_t x, y;
	size_t width, height;
	component_t* root;
} rund_app_t;

#ifdef __cplusplus
extern "C" {
#endif

void run_app(const rund_app_t* app);

#ifdef __cplusplus
}
#endif
