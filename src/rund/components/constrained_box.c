#include <rund/components/constrained_box.h>
#include <rund/memory/gc.h>
#include "../log.h"
#include <rund/memory/allocation.h>

#include <stdlib.h>
#include <string.h>

static const box_constraints_t default_constraints = {
	.min_width = 0,
	.min_height = 0,
	.max_width = -1,
	.max_height = -1
};

constrained_box_t* constrained_box_create(constrained_box_attributes_t attributes)
{
    constrained_box_t* component = (constrained_box_t*)gc_alloc(sizeof(constrained_box_t));

	component->attributes.child = attributes.child;
	component->attributes.constraints = attributes.constraints ?: clone(&default_constraints, sizeof(box_constraints_t));

	component->base.type = CONSTRAINED_BOX;
    component->base.isFlexible = false;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {ConstrainedBox}\t[%s]", 0, attributes.id);
	return component;
}