#include <rund/components/container.h>
#include "../log.h"
#include <rund/utils/allocation.h>

#include <stdlib.h>
#include <string.h>

static const container_decoration_t def_dec_contaner = { .color = 0xFF0000 };

container_t* container_create(container_attributes_t attributes)
{
	container_t* component = (container_t*)malloc(sizeof(container_t));

	component->attributes.child = attributes.child;
	component->attributes.width = attributes.width ?: alloc_size(0);
	component->attributes.height = attributes.height ?: alloc_size(0);
	component->attributes.decoration = attributes.decoration ?: clone(&def_dec_contaner, sizeof(container_decoration_t));

	component->base.type = CONTAINER;
	component->base.isFlexible = false;
	memcpy(component->base.id, attributes.id, ID_LEN);

	TRACE("Component Created {Container}", 0);
	return component;
}
