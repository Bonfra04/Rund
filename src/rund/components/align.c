#include <rund/components/align.h>
#include "../log.h"
#include <rund/utils/allocation.h>

#include <stdlib.h>
#include <string.h>

static const alignment_t def_alignment = { .x = 0.0f, .y = 0.0f };

align_t* align_create(align_attributes_t attributes)
{
    align_t* component = (align_t*)malloc(sizeof(align_t));

    component->attributes.child = attributes.child;
    component->attributes.alignment = attributes.alignment ?: clone(&def_alignment, sizeof(alignment_t));

	component->base.type = ALIGN;
    component->base.isFlexible = false;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Align}", 0);
	return component;
}