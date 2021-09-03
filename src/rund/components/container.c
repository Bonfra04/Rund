#include <rund/components/container.h>
#include <stdlib.h>
#include "../log.h"

container_decoration_t def_dec_contaner = { .color = 0xFF0000 };

container_t* container_create(component_t* child, size_t width, size_t height, container_decoration_t decoration)
{
	container_t* component = (container_t*)malloc(sizeof(container_t));

	component->child = child;
	component->width = width;
	component->height = height;
	component->decoration = decoration;

	TRACE("Component Created {Container}", 0);

	component->base.type = CONTAINER;
	component->base.isFlexible = false;
	return component;
}
