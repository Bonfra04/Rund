#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/log.h>
#include <rund/utils/max.h>
#include <rund/renderer.h>

#include <string.h>

static draw_data_t draw_align(const align_t* align, const build_context_t context, uint64_t deepness);

align_t* align_create(align_attributes_t attributes)
{
    align_t* component = (align_t*)gc_alloc(sizeof(align_t));

    component->child = attributes.child;
    component->alignment = attributes.alignment ?: Alignment(0.0f, 0.0f);

    component->base.draw_func = DrawFunc(draw_align);
    component->base.flags = FLAG_NONE;
	memcpy(component->base.id, attributes.id, ID_LEN);

    TRACE("Component Created {Align}\t[%s]", 0, attributes.id);
	return component;
}

static draw_data_t draw_align(const align_t* align, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    build_context_t child_context = {
        .max_height = context.max_height, .min_height = context.min_height,
        .max_width = context.max_width, .min_width = context.min_width,
        .backbuffer = buffer_create(context.max_width, context.max_height)
    };

    draw_data_t child_data = draw_component(align->child, (component_t*)align, child_context, deepness + 1);

    float align_x = (align->alignment->x + 1) / 2;
    float align_y = (align->alignment->y + 1) / 2;

    uint64_t xp = context.max_width * align_x;
    uint64_t yp = context.max_height * align_y;

    uint64_t xc = child_data.dims.width * align_x;
    uint64_t yc = child_data.dims.height * align_y;

    uint64_t y_offset = yp - yc;
    uint64_t x_offset = xp - xc;

    data.dims = child_data.dims;
    data.coords = (coord_t){ x_offset, y_offset };
    data.childs = child_data.childs ?: vector_create(widget_position_t);

    widget_position_t child_pos = {
        .dimensions = child_data.dims,
        .coords = child_data.coords,
        .z = deepness,
        .component = align->child
    };

    vector_push_back(data.childs, child_pos);

    for (uint64_t y = 0; y < child_data.dims.height; y++)
        for (uint64_t x = 0; x < child_data.dims.width; x++)
        {
            color_t* bg_pixel = &context.backbuffer.data[(y + y_offset) * context.backbuffer.width + (x + x_offset)];
            color_t* child_pixel = &child_context.backbuffer.data[y * child_context.backbuffer.width + x];
            *bg_pixel = blend(*bg_pixel, *child_pixel);
        }

    buffer_destroy(&child_context.backbuffer);

    return data;
}