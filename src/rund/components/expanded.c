#include <rund/components/expanded.h>
#include "../log.h"
#include <rund/utils/allocation.h>

#include <stdlib.h>
#include <string.h>

expanded_t* expanded_create(expanded_attributes_t attributes)
{
	expanded_t* component = (expanded_t*)malloc(sizeof(expanded_t));

	component->attributes.child = attributes.child;

	component->base.type = EXPANDED;
    component->base.isFlexible = true;
    component->base.flex = attributes.flex ?: alloc_uint64(1);
	memcpy(component->base.id, attributes.id, ID_LEN);

	TRACE("Component Created {Expanded}", 0);
	return component;
}
