#include <rund/components/listener.h>
#include <stdlib.h>
#include "../log.h"

listener_t* listener_create(component_t* child, handlers_t handlers)
{
	listener_t* component = (listener_t*)malloc(sizeof(listener_t));

	component->child = child;
    component->handlers = handlers;

	TRACE("Component Created {Listener}", 0);

	component->base.type = LISTENER;
	component->base.isFlexible = false;
	return component;
}
