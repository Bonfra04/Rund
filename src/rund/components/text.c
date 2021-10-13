#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>

text_t* text_create(const char* text)
{
    text_t* component = (text_t*)gc_alloc(sizeof(text_t));
    
	TRACE("Component Created {Text}", 0);

	component->text = text;

    component->base.type = TEXT;
	component->base.isFlexible = false;
    return component;
}