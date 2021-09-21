#include <rund/components/text.h>
#include <stdlib.h>
#include "../log.h"

text_t* text_create(const char* text)
{
    text_t* component = (text_t*)malloc(sizeof(text_t));
    
	TRACE("Component Created {Text}", 0);

	component->text = text;

    component->base.type = TEXT;
	component->base.isFlexible = false;
    return component;
}