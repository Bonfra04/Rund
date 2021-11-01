#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>

container_t* container_create(container_attributes_t attributes)
{
	container_t* component = (container_t*)gc_alloc(sizeof(container_t));

	component->child = attributes.child;
	component->width = attributes.width ?: attributes.child ? 0 : Val((size_t)0);
	component->height = attributes.height ?: attributes.child ? 0 : Val((size_t)0);
	component->color = attributes.color ?: Val((color_t)0x00000000);

	component->base.type = CONTAINER;
	memcpy(component->base.id, attributes.id, ID_LEN);

	TRACE("Component Created {Container}\t[%s]", 0, attributes.id);
	return component;
}
