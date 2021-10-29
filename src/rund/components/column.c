#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>

static const column_decoration_t def_dec_column = { .color = 0x00000000 };

column_t* column_create(column_attributes_t attributes)
{
	column_t* component = (column_t*)gc_alloc(sizeof(column_t));

	component->attributes.children = attributes.children;
	component->attributes.decoration = attributes.decoration ?: clone(&def_dec_column, sizeof(column_decoration_t));

	component->base.type = COLUMN;
	component->base.isFlexible = false;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Column}\t[%s]", 0, attributes.id);
	return component;
}
