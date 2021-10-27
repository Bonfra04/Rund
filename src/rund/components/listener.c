#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>

#include <string.h>


listener_t* listener_create(listener_attributes_t attributes)
{
    listener_t* component = (listener_t*)gc_alloc(sizeof(listener_t));

    component->attributes.child = attributes.child;
    component->attributes.handlers = attributes.handlers ?: Handlers(
        .on_pointer_down = 0,
        .on_pointer_up = 0,
        .on_key_down = 0,
        .on_key_up = 0
    );

    component->base.type = LISTENER;
    component->base.isFlexible = false;
    memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Listener}\t[%s]", 0, attributes.id);
    return component;
}
