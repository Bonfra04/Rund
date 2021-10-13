#include <rund/components/expanded.h>
#include <rund/memory/gc.h>
#include "../log.h"
#include <rund/memory/allocation.h>

#include <stdlib.h>
#include <string.h>

expanded_t* expanded_create(expanded_attributes_t attributes)
{
	expanded_t* component = (expanded_t*)gc_alloc(sizeof(expanded_t));

	component->attributes.child = attributes.child;

	component->base.type = EXPANDED;
    component->base.isFlexible = true;
    component->base.flex = attributes.flex ?: Val(uint64_t, 1);
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Expanded}\t[%s]", 0, attributes.id);
	return component;
}
