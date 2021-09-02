#include "row.h"
#include <stdlib.h>
#include "../log.h"

row_decoration_t def_dec_row = { .spacing = 0, .alignment_x = axis_start, .alignment_y = axis_center };

row_t* row_create(component_t** children, uint64_t children_count, row_decoration_t decoration)
{
	row_t* component = (row_t*)malloc(sizeof(row_t));

	component->children = children;
	component->decoration = decoration;
	component->children_count = children_count;

	TRACE("Component Created {ROW}", 0);

	component->base.type = ROW;
	component->base.isFlexible = false;
	return component;
}
