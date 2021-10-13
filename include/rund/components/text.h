#pragma once

#include "component.h"

typedef struct text
{
	component_t base;
    const char* text;
} __attribute__((packed)) text_t;

#ifdef __cplusplus
extern "C" {
#endif

text_t* text_create(const char* text);

#ifdef __cplusplus
}
#endif

#include "../vrg.h"

#define Text(text) ((component_t*)text_create(text))
