#pragma once

#include "component.h"
#include <stddef.h>

typedef struct row_decoration
{
	color_t color;
} row_decoration_t;

#define RowDec (row_decoration_t)

typedef struct row
{
	component_t base;
	row_decoration_t decoration;
	component_t** children;
	uint64_t children_count;
} __attribute__((packed)) row_t;

#ifdef __cplusplus
extern "C" {
#endif

extern row_decoration_t def_dec_row;

row_t* row_create(component_t** children, uint64_t children_count, row_decoration_t decoration);

#ifdef __cplusplus
}
#endif

#include "../vrg.h"

#define Row2(children, children_count) ((component_t*)row_create(children, children_count, def_dec_row))
#define Row3(children, children_count, decoration) ((component_t*)row_create(children, children_count, decoration))

#define Row(...) vrg(Row, __VA_ARGS__)