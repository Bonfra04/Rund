#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/utils/max.h>

#include <string.h>

static draw_data_t draw_flexible(const flexible_t* flexible, const build_context_t context, uint64_t deepness);

flexible_t* flexible_create(flexible_attributes_t attributes)
{
	flexible_t* component = (flexible_t*)gc_alloc(sizeof(flexible_t));

	component->attributes.flex = attributes.flex ?: Val((uint64_t)1);
	component->attributes.child = attributes.child;

    component->base.draw_func = DrawFunc(draw_flexible);
    component->base.flags = FLAG_FLEXIBLE;
	memcpy(component->base.id, attributes.id, ID_LEN);

	return component;
}

static draw_data_t draw_flexible(const flexible_t* flexible, build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;
    data.dims.width = context.max_width;
    data.dims.height = context.max_height;

    const flexible_attributes_t* attributes = &flexible->attributes;
    
    build_context_t child_context = {
        .max_height = context.max_height, .min_height = context.max_height,
        .max_width = context.max_width, .min_width = context.max_width,
        .backbuffer = context.backbuffer
    };

    draw_data_t child_data = draw_component(attributes->child, (component_t*)flexible, child_context, deepness + 1);

    data.childs = child_data.childs ?: vector_create(widget_position_t);

    widget_position_t child_pos = {
        .dimensions = child_data.dims,
        .coords = child_data.coords,
        .z = deepness,
        .component = attributes->child
    };

    vector_push_back(data.childs, child_pos);

    return data;
}
