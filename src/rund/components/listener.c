#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>
#include <rund/utils/max.h>
#include <rund/renderer.h>

#include <string.h>

static draw_data_t draw_listener(const listener_t* listener, const build_context_t context, uint64_t deepness);

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

    component->base.draw_func = DrawFunc(draw_listener);
    component->base.flags = FLAG_LISTENER;
    memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Listener}\t[%s]", 0, attributes.id);
    return component;
}

static draw_data_t draw_listener(const listener_t* listener, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    const listener_attributes_t* attributes = &(listener->attributes);

    draw_data_t child_data = draw_component(attributes->child, (component_t*)listener, context, deepness + 1);

    widget_position_t child_pos = {
        .dimensions = child_data.dims,
        .coords = {0, 0},
        .z = deepness,
        .component = attributes->child
    };

    data.dims = child_data.dims;
    data.coords = child_data.coords;
    data.childs = child_data.childs ?: vector_create(widget_position_t);
    vector_push_back(data.childs, child_pos);

    return data;
}
