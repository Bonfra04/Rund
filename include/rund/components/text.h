#pragma once

#include <rund/component.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct text_attributes
{
	char id[ID_LEN];
	const wchar_t* text;
    float* font_size;
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