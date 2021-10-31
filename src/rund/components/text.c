#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>

text_t* text_create(text_attributes_t attributes)
{
    text_t* component = (text_t*)gc_alloc(sizeof(text_t));

	component->attributes.text = attributes.text;
    component->attributes.font_size = attributes.font_size ?: Val((float)14.0f);

	component->base.type = TEXT;
	memcpy(component->base.id, attributes.id, ID_LEN);

	TRACE("Component Created {TEXT}\t[%s]", 0, attributes.id);
	return component;
}