#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>

flexible_t* flexible_create(flexible_attributes_t attributes)
{
	flexible_t* component = (flexible_t*)gc_alloc(sizeof(flexible_t));

	component->attributes.flex = attributes.flex ?: Val((uint64_t)1);
	component->attributes.child = attributes.child;

	component->base.type = FLEXIBLE;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Flexible}\t[%s]", 0, attributes.id);
	return component;
}
