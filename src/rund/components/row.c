#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>

static const row_decoration_t def_dec_row = { .color = 0x000000 };

row_t* row_create(row_attributes_t attributes)
{
	row_t* component = (row_t*)gc_alloc(sizeof(row_t));

	component->attributes.children = attributes.children;
	component->attributes.decoration = attributes.decoration ?: clone(&def_dec_row, sizeof(row_decoration_t));

	component->base.type = ROW;
	component->base.isFlexible = false;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Row}\t[%s]", 0, attributes.id);
	return component;
}
