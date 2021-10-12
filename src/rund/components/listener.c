#include <rund/components/listener.h>
#include "../log.h"
#include <rund/utils/allocation.h>

#include <stdlib.h>
#include <string.h>

const static handlers_t def_handlers = {
	.on_pointer_down = 0,
	.on_pointer_up = 0
};

listener_t* listener_create(listener_attributes_t attributes)
{
	listener_t* component = (listener_t*)malloc(sizeof(listener_t));

	component->attributes.child = attributes.child;
    component->attributes.handlers = attributes.handlers ?: clone(&def_handlers, sizeof(handlers_t));

	component->base.type = LISTENER;
	component->base.isFlexible = false;
	memcpy(component->base.id, attributes.id, ID_LEN);

	TRACE("Component Created {Listener}", 0);
	return component;
}
