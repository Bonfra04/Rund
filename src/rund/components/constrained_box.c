#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>
#include <rund/utils/max.h>

#include <string.h>

static draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context, uint64_t deepness);

constrained_box_t* constrained_box_create(constrained_box_attributes_t attributes)
{
    constrained_box_t* component = (constrained_box_t*)gc_alloc(sizeof(constrained_box_t));

	component->attributes.child = attributes.child;
	component->attributes.constraints = attributes.constraints ?: BoxConstraints(0, 0, -1, -1);

    component->base.draw_func = DrawFunc(draw_constrained_box);
    component->base.flags = FLAG_NONE;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {ConstrainedBox}\t[%s]", 0, attributes.id);
	return component;
}

static draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context, uint64_t deepness)
{
    const constrained_box_attributes_t* attributes = &(constrained_box->attributes);

    build_context_t child_context = {
        .max_height = min(context.max_height, attributes->constraints->max_height),
        .min_height = max(context.min_height, attributes->constraints->min_height),
        .max_width = min(context.max_width, attributes->constraints->max_width),
        .min_width = max(context.min_width, attributes->constraints->min_width),
        .backbuffer = context.backbuffer
    };
    
    draw_data_t data = draw_component(attributes->child, (component_t*)constrained_box, child_context, deepness + 1);

    if(data.childs == NULL)
        data.childs = vector_create(widget_position_t);

    widget_position_t child_pos = {
        .dimensions = data.dims,
        .coords = data.coords,
        .z = deepness,
        .component = attributes->child
    };

    vector_push_back(data.childs, child_pos);

    return data;
}