#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>

static const alignment_t def_alignment = { .x = 0.0f, .y = 0.0f };

align_t* align_create(align_attributes_t attributes)
{
    align_t* component = (align_t*)gc_alloc(sizeof(align_t));

    component->attributes.child = attributes.child;
    component->attributes.alignment = attributes.alignment ?: Alignment(0.0f, 0.0f);

	component->base.type = ALIGN;
    component->base.isFlexible = false;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Align}\t[%s]", 0, attributes.id);
	return component;
}