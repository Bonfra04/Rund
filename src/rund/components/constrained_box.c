#include <rund/components/constrained_box.h>
#include <stdlib.h>
#include "../log.h"

constrained_box_t* constrained_box_create(component_t* child, box_constraints_t constraints)
{
    constrained_box_t* component = (constrained_box_t*)malloc(sizeof(constrained_box_t));

	component->constraints = constraints;
	component->child = child;

	TRACE("Component Created {ConstrainedBox}", 0);

	component->base.type = CONSTRAINED_BOX;
    component->base.isFlexible = false;
	return component;
}