#include <rund/components/expanded.h>
#include <stdlib.h>
#include "../log.h"

expanded_t* expanded_create(component_t* child, uint64_t flex)
{
	expanded_t* component = (expanded_t*)malloc(sizeof(expanded_t));

	component->child = child;

	TRACE("Component Created {Expanded}", 0);

	component->base.type = EXPANDED;
    component->base.isFlexible = true;
    component->base.flex = flex;
	return component;
}
