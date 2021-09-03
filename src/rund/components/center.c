#include <rund/components/center.h>
#include <stdlib.h>
#include "../log.h"

center_t* center_create(component_t* child)
{
    center_t* component = (center_t*)malloc(sizeof(center_t));
    
	TRACE("Component Created {Center}", 0);

	component->align.child = child;
    component->align.alignment = Alignment(0.0f, 0.0f);

	component->align.base.type = ALIGN;
	component->align.base.isFlexible = false;

    component->base.type = CENTER;
	component->base.isFlexible = false;
    return component;
}