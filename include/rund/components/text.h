#pragma once

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct text_attributes
{
	char id[ID_LEN];
	const char* text;
    size_t* font_size;
} text_attributes_t;

typedef struct text
{
	component_t base;
    text_attributes_t attributes;
} __attribute__((packed)) text_t;

text_t* text_create(text_attributes_t attributes);

#define Text(...)((component_t*)text_create((text_attributes_t){__VA_ARGS__}))

#ifdef __cplusplus
}
#endif