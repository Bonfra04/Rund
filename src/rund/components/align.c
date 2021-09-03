#include <rund/components/align.h>
#include <stdlib.h>
#include "../log.h"

align_t* align_create(component_t* child, alignment_t alignment)
{
    align_t* component = (align_t*)malloc(sizeof(align_t));

	alignment.x = alignment.x > 1.0f ? 1.0f : alignment.x < -1.0f ? -1.0f : alignment.x;
	alignment.y = alignment.y > 1.0f ? 1.0f : alignment.y < -1.0f ? -1.0f : alignment.y;

	component->child = child;
    component->alignment = alignment;

	TRACE("Component Created {Align}", 0);

	component->base.type = ALIGN;
    component->base.isFlexible = false;
	return component;
}