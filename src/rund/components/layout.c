#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>

layout_t* layout_create(layout_attributes_t attributes)
{
	layout_t* component = (layout_t*)gc_alloc(sizeof(layout_t));

	component->attributes.children = attributes.children;
	component->attributes.laying_style = attributes.laying_style ?: Stack;
	
	component->base.type = LAYOUT;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Layout}\t[%s]", 0, attributes.id);
	return component;
}
