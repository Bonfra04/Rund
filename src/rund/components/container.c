#include <rund/components/container.h>
#include <rund/memory/gc.h>
#include "../log.h"
#include <rund/memory/allocation.h>

#include <stdlib.h>
#include <string.h>

static const container_decoration_t def_dec_contaner = { .color = 0xFF0000 };

container_t* container_create(container_attributes_t attributes)
{
	container_t* component = (container_t*)gc_alloc(sizeof(container_t));

	component->attributes.child = attributes.child;
	component->attributes.width = attributes.width ?: Val(size_t, 0);
	component->attributes.height = attributes.height ?: Val(size_t, 0);
	component->attributes.decoration = attributes.decoration ?: ContainerDec(.color = 0xFF0000);

	component->base.type = CONTAINER;
	component->base.isFlexible = false;
	memcpy(component->base.id, attributes.id, ID_LEN);

	TRACE("Component Created {Container}\t[%s]", 0, attributes.id);
	return component;
}
