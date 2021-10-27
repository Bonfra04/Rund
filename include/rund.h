#pragma once

#include <rund/components.h>
#include <rund/keycodes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rund_app
{
	char title[50];
	uint64_t x, y;
	size_t width, height;
	component_t* root;
} rund_app_t;

void run_app(const rund_app_t* app);
component_t* rund_get_component(char id[ID_LEN]);
void acquire_focus(listener_t* listener);

#define Ref(x) (*x)
#define Val(type, x) (alloc_##type(x))

#ifdef __cplusplus
}
#endif
