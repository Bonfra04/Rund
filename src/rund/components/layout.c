#include <rund.h>
#include <rund/memory/gc.h>
#include <rund/utils/max.h>

#include <string.h>

static draw_data_t draw_layout(const layout_t* layout, const build_context_t context, uint64_t deepness);

layout_t* layout_create(layout_attributes_t attributes)
{
	layout_t* component = (layout_t*)gc_alloc(sizeof(layout_t));

	component->attributes.children = attributes.children;
	component->attributes.laying_style = attributes.laying_style ?: Stack;
	
    component->base.draw_func = DrawFunc(draw_layout);
    component->base.flags = FLAG_NONE;
	memcpy(component->base.id, attributes.id, ID_LEN);

	return component;
}

static draw_data_t draw_layout(const layout_t* layout, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;
    data.childs = vector_create(widget_position_t);

    const layout_attributes_t* attributes = &(layout->attributes);

    size_t num_childs = attributes->children->length;

    buffer_t backbuffers[num_childs];
    draw_data_t datas[num_childs];
    memset(backbuffers, 0, sizeof(buffer_t) * num_childs);
    memset(datas, 0, sizeof(draw_data_t) * num_childs);

    uint64_t limiting_dimension;
    uint64_t flex_count = 0;
    uint64_t flexible_space =
        *attributes->laying_style == Column ? context.max_height :
        *attributes->laying_style == Row ? context.max_width :
        0;

    // draw unflexibles
    for(uint64_t i = 0; i < num_childs; i++)
    {
        component_t* child = attributes->children->components[i];
        if(child->flags & FLAG_FLEXIBLE)
        {
            flex_count += *((flexible_t*)child)->attributes.flex;
            continue;
        }

        backbuffers[i] = buffer_create(context.max_width, context.max_height);
        build_context_t child_context = {
            .max_width = context.max_width, .max_height = context.max_height,
            .min_width = context.min_width, .min_height = context.min_height,
            .backbuffer = backbuffers[i]
        };

        datas[i] = draw_component(child, (component_t*)layout, child_context, deepness + (num_childs - i));
    
        if(*attributes->laying_style == Column)
        {
            uint64_t furthest_point = datas[i].dims.width + datas[i].coords.x;
            if(i == 0 || furthest_point > limiting_dimension)
                limiting_dimension = furthest_point;
            flexible_space -= datas[i].dims.height;
        }
        else if(*attributes->laying_style == Row)
        {
            uint64_t furthest_point = datas[i].dims.height + datas[i].coords.y;
            if(i == 0 || furthest_point > limiting_dimension)
                limiting_dimension = furthest_point;
            flexible_space -= datas[i].dims.width;
        }
    }

    // draw flexibles
    for(int i = 0; i < num_childs; i++)
    {
        flexible_t* child = (flexible_t*)attributes->children->components[i];
        if((child->base.flags & FLAG_FLEXIBLE) == 0)
            continue;

        uint64_t width =
            *layout->attributes.laying_style != Row ? context.max_width :
            flexible_space / flex_count * *child->attributes.flex;
        uint64_t height =
            *layout->attributes.laying_style != Column ? context.max_height :
            flexible_space / flex_count * *child->attributes.flex;

        backbuffers[i] = buffer_create(context.max_width, context.max_height);
        build_context_t child_context = {
            .max_width = width, .max_height = height,
            .min_width = min(width, context.min_width), .min_height = min(height, context.min_height),
            .backbuffer = backbuffers[i]
        };

        datas[i] = draw_component((component_t*)child, (component_t*)layout, child_context, deepness + (num_childs - i));
    }

    coord_t top_left = { .x = context.max_width, .y = context.max_height };
    coord_t bottom_right = { .x = 0, .y = 0 };
    uint64_t advance_x = 0;
    uint64_t advance_y = 0;

    // render childs
    size_t ongoing_deepness = 0;
    const buffer_t* main_buffer = &context.backbuffer;
    for(uint64_t i = 0; i < num_childs; i++)
    {
        if(backbuffers[i].width == 0 || backbuffers[i].height == 0 || backbuffers[i].data == NULL)
            continue;

        coord_t child_top_left = {
            .x = datas[i].coords.x + advance_x,
            .y = datas[i].coords.y + advance_y
        };
        coord_t child_bottom_right = {
            .x = child_top_left.x + datas[i].dims.width,
            .y = child_top_left.y + datas[i].dims.height
        };

        top_left.x = min(top_left.x, child_top_left.x);
        top_left.y = min(top_left.y, child_top_left.y);
        bottom_right.x = max(bottom_right.x, child_bottom_right.x);
        bottom_right.y = max(bottom_right.y, child_bottom_right.y);

        buffer_t* child_buffer = &backbuffers[i];

        size_t deepest_child = 0;
        if(datas[i].childs)
        {
            for(size_t c = 0; c < vector_size(datas[i].childs); c++)
            {
                widget_position_t child_pos = ((widget_position_t*)datas[i].childs)[c];
                child_pos.coords.x += child_top_left.x;
                child_pos.coords.y += child_top_left.y;
                vector_push_back(data.childs, child_pos);
                if(child_pos.z > deepest_child)
                    deepest_child = child_pos.z;
            }
            vector_destroy(datas[i].childs);
        }
        ongoing_deepness += deepest_child;

        widget_position_t child_pos = {
            .dimensions = datas[i].dims,
            .coords = child_top_left,
            .z = deepness + ongoing_deepness,
            .component = attributes->children->components[i]
        };
        vector_push_back(data.childs, child_pos);

        for (uint64_t y = 0; y < datas[i].dims.height; y++)
            for (uint64_t x = 0; x < datas[i].dims.width; x++)
            {
                color_t* main_pixel = &main_buffer->data[(y + child_top_left.y) * main_buffer->width + (x + child_top_left.x)]; 
                color_t* child_pixel = &child_buffer->data[(y + datas[i].coords.y) * child_buffer->width + (x + datas[i].coords.x)];
                *main_pixel = blend(*main_pixel, *child_pixel);
            }
    
        buffer_destroy(child_buffer);

        if(*layout->attributes.laying_style == Column)
            advance_y += datas[i].dims.height;
        else if(*layout->attributes.laying_style == Row)
            advance_x += datas[i].dims.width;
    }

    data.dims.width = bottom_right.x - top_left.x;
    data.dims.height = bottom_right.y - top_left.y;
    data.coords = top_left;

    return data;
}