#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/utils/max.h>

#include <string.h>

static draw_data_t draw_container(const container_t* container, const build_context_t context, uint64_t deepness);

container_t* container_create(container_attributes_t attributes)
{
	container_t* component = (container_t*)gc_alloc(sizeof(container_t));

	component->child = attributes.child;
	component->width = attributes.width ?: attributes.child ? 0 : Val((size_t)0);
	component->height = attributes.height ?: attributes.child ? 0 : Val((size_t)0);
	component->color = attributes.color ?: Val((color_t)0x00000000);

	component->base.draw_func = DrawFunc(draw_container);
    component->base.flags = FLAG_NONE;
	memcpy(component->base.id, attributes.id, ID_LEN);

	return component;
}

static draw_data_t draw_container(const container_t* container, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    data.dims.width = context.max_width;
    data.dims.height = context.max_height;

    if(container->width && *container->width < data.dims.width)
        data.dims.width = max(*container->width, context.min_width);
    if(container->height && *container->height < data.dims.height)
        data.dims.height = max(*container->height, context.min_height);

    for (uint64_t y = 0; y < data.dims.height; y++)
        for (uint64_t x = 0; x < data.dims.width; x++)
        {
            color_t* bg_pixel = &context.backbuffer.data[y * context.backbuffer.width + x];
            *bg_pixel = blend(*bg_pixel, *container->color);
        }

    if(container->child != NULL)
    {
        build_context_t child_context = {
            .max_width = data.dims.width, .max_height = data.dims.height,
            .min_width = context.min_width, .min_height = context.min_height,
            .backbuffer = buffer_create(data.dims.width, data.dims.height)
        };

        draw_data_t child_data = draw_component(container->child, (component_t*)container, child_context, deepness + 1);

        if(!container->width)
            data.dims.width = child_data.dims.width;
        if(!container->height)
            data.dims.height = child_data.dims.height;

        widget_position_t child_pos = {
            .dimensions = child_data.dims,
            .coords = child_data.coords,
            .z = deepness,
            .component = container->child
        };

        data.childs = child_data.childs ?: vector_create(widget_position_t);
        vector_push_back(data.childs, child_pos);

        for(size_t x = 0; x < child_data.dims.width; x++)
            for(size_t y = 0; y < child_data.dims.height; y++)
            {
                color_t* bg_pixel = &context.backbuffer.data[y * context.backbuffer.width + x];
                color_t* child_pixel = &child_context.backbuffer.data[(child_data.coords.y + y) * child_context.backbuffer.width + (child_data.coords.x + x)];
                *bg_pixel = blend(*bg_pixel, *child_pixel);
            }

        buffer_destroy(&child_context.backbuffer);
    }

    return data;
}
