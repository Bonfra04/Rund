#include <rund.h>
#include <rund/core.h>
#include <rund/wrapper.h>
#include <rund/utils/vector.h>
#include <rund/log.h>
#include <rund/renderer.h>
#include <rund/utils/max.h>

#include <string.h>
#include <wchar.h>

extern int rund_main();

int main(int argc, char* argv[])
{
    gc_t gc;
    gc_init(&gc, &argc);

    int ret_val = rund_main();

    gc_collect();
    gc_dump();
    gc_stop();

    return ret_val;
}

draw_data_t draw_component(const component_t* component, const component_t* parent, const build_context_t context, uint64_t deepness);
draw_data_t draw_container(const container_t* container, const build_context_t context, uint64_t deepness);
draw_data_t draw_flexible(const flexible_t* flexible, const build_context_t context, uint64_t deepness);
draw_data_t draw_layout(const layout_t* layout, const build_context_t context, uint64_t deepness);
draw_data_t draw_align(const align_t* align, const build_context_t context, uint64_t deepness);
draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context, uint64_t deepness);
draw_data_t draw_listener(const listener_t* listener, const build_context_t context, uint64_t deepness);
draw_data_t draw_text(const text_t* text, const build_context_t context, uint64_t deepness);

typedef struct widget_position
{
    dimension_t dimensions;
    coord_t coords;
    uint64_t z;
    component_t* component;
} widget_position_t;

static vector_t widgets;
static uint64_t deepness;
static listener_t* focus_node;

static listener_t* get_listener(uint64_t x, uint64_t y)
{
    if(!widgets)
        return NULL;

    widget_position_t* target = NULL;
    for(size_t i = 0; i < vector_size(widgets); i++)
    {
        widget_position_t* widget = &((widget_position_t*)widgets)[i];
        if(x >= widget->coords.x && x < widget->coords.x + widget->dimensions.width &&
            y >= widget->coords.y && y < widget->coords.y + widget->dimensions.height)
            if(target == NULL || widget->z > target->z)
                target = widget;
    }

    if(target == NULL)
        return NULL;

    component_t* component = target->component;
    while(component != NULL && component->type != LISTENER)
        component = component->parent;

    if(component == NULL)
        return NULL;

    return (listener_t*)component;
}

static void on_mouse_up(uint64_t x, uint64_t y, uint8_t button)
{
    listener_t* listener = get_listener(x, y);
    if(listener == NULL)
        return;

    if(listener->attributes.handlers->on_pointer_up)
        listener->attributes.handlers->on_pointer_up((component_t*)listener);
}

static void on_mouse_down(uint64_t x, uint64_t y, uint8_t button)
{
    listener_t* listener = get_listener(x, y);
    if(listener == NULL)
        return;

    if(listener->attributes.handlers->on_pointer_down)
        listener->attributes.handlers->on_pointer_down((component_t*)listener);
}

static void on_key_down(uint16_t key)
{
    if(focus_node == NULL)
        return;

    if(focus_node->attributes.handlers->on_key_down)
        focus_node->attributes.handlers->on_key_down((component_t*)focus_node, key);
}

static void on_key_up(uint16_t key)
{
    if(focus_node == NULL)
        return;

    if(focus_node->attributes.handlers->on_key_up)
        focus_node->attributes.handlers->on_key_up((component_t*)focus_node, key);
}

static rund_app_t* app;

static void on_resize(uint64_t width, uint64_t height)
{
    app->width = width;
    app->height = height;
}

void run_app(rund_app_t* rund_app)
{
    app = rund_app;

    widgets = 0;
    focus_node = 0;

    renderer_init();
    create_window(app, (events_t){
        .mouse_down = on_mouse_down, .mouse_up = on_mouse_up,
        .key_down = on_key_down, .key_up = on_key_up,
        .resize = on_resize
    });

    do
    {
        if(widgets)
            vector_destroy(widgets);
        deepness = 0;

        build_context_t context = {
            .max_width = app->width, .max_height = app->height,
            .min_width = 0, .min_height = 0,
            .backbuffer = buffer_create(app->width, app->height)
        };

        draw_data_t data = draw_component(app->root, NULL, context, 1);
        widget_position_t root_pos = {
            .dimensions = data.dims,
            .coords = data.coords,
            .z = 0,
            .component = app->root
        };

        widgets = data.childs ?: vector_create(widget_position_t);
        vector_push_back(widgets, root_pos);

        for(uint64_t y = 0; y < data.dims.height; y++)
            for(uint64_t x = 0; x < data.dims.width; x++)
                put_pixel(x + data.coords.x, y + data.coords.y, context.backbuffer.data[(y + data.coords.y) * app->width + (x + data.coords.x)]);

        buffer_destroy(&context.backbuffer);

        flush();
    } while (event_loop());

    TRACE("Rund app terminated\n", 0);
}

component_t* rund_get_component(char id[ID_LEN])
{
    size_t size = vector_size(widgets);
    widget_position_t* vector = (widget_position_t*)widgets;
    
    for(size_t i = 0; i < size; i++)
        if(strcmp(vector[i].component->id, id) == 0)
            return vector[i].component;
    
    return NULL;
}

void acquire_focus(listener_t* listener)
{
    if(listener == NULL)
        return;

    focus_node = listener;
}

draw_data_t draw_component(const component_t* component, const component_t* parent, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    if(component)
    {
        component_t* c = (component_t*)component;
        c->parent = (component_t*)parent;

        switch (component->type)
        {
        case EMPTY:
            break;
        case CONTAINER:
            data = draw_container((const container_t*)component, context, deepness);
            break;
        case LAYOUT:
            data = draw_layout((const layout_t*)component, context, deepness);
            break;
        case FLEXIBLE:
            data = draw_flexible((const flexible_t*)component, context, deepness);
            break;
        case ALIGN:
            data = draw_align((const align_t*)component, context, deepness);
            break;
        case CONSTRAINED_BOX:
            data = draw_constrained_box((const constrained_box_t*)component, context, deepness);
            break;
        case LISTENER:
            data = draw_listener((const listener_t*)component, context, deepness);
            break;
        case TEXT:
            data = draw_text((const text_t*)component, context, deepness);
            break;
        default:
            ERROR("Unrecognized component type: [%d]", 0, (int)component->type);
            break;
        }
    }

    return data;
}

draw_data_t draw_container(const container_t* container, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    const container_attributes_t* attributes = &(container->attributes);

    data.dims.width = context.max_width;
    data.dims.height = context.max_height;

    if(attributes->width && *attributes->width < data.dims.width)
        data.dims.width = max(*attributes->width, context.min_width);
    if(attributes->height && *attributes->height < data.dims.height)
        data.dims.height = max(*attributes->height, context.min_height);

    for (uint64_t y = 0; y < data.dims.height; y++)
        for (uint64_t x = 0; x < data.dims.width; x++)
        {
            color_t* bg_pixel = &context.backbuffer.data[y * context.backbuffer.width + x];
            *bg_pixel = blend(*bg_pixel, *attributes->color);
        }

    if(attributes->child != NULL)
    {
        build_context_t child_context = {
            .max_width = data.dims.width, .max_height = data.dims.height,
            .min_width = context.min_width, .min_height = context.min_height,
            .backbuffer = buffer_create(data.dims.width, data.dims.height)
        };

        draw_data_t child_data = draw_component(attributes->child, (component_t*)container, child_context, deepness + 1);

        if(!attributes->width)
            data.dims.width = child_data.dims.width;
        if(!attributes->height)
            data.dims.height = child_data.dims.height;

        widget_position_t child_pos = {
            .dimensions = child_data.dims,
            .coords = child_data.coords,
            .z = deepness,
            .component = attributes->child
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

draw_data_t draw_layout(const layout_t* layout, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;
    data.childs = vector_create(widget_position_t);

    const layout_attributes_t* attributes = &(layout->attributes);

    size_t num_childs = attributes->children->length;

    buffer_t backbuffers[num_childs];
    draw_data_t datas[num_childs];
    memset(backbuffers, 0, sizeof(buffer_t) * num_childs);
    memset(datas, 0, sizeof(draw_data_t) * num_childs);

    // draw unflexibles
    for(uint64_t i = 0; i < num_childs; i++)
    {
        component_t* child = attributes->children->components[i];
        if(child->type == FLEXIBLE)
            continue;

        backbuffers[i] = buffer_create(context.max_width, context.max_height);
        build_context_t child_context = {
            .max_width = context.max_width, .max_height = context.max_height,
            .min_width = context.min_width, .min_height = context.min_height,
            .backbuffer = backbuffers[i]
        };

        datas[i] = draw_component(child, (component_t*)layout, child_context, deepness + (num_childs - i));
    }

    // draw flexibles
    for(int i = 0; i < num_childs; i++)
    {
        component_t* child = attributes->children->components[i];
        if(child->type != FLEXIBLE)
            continue;

        backbuffers[i] = buffer_create(context.max_width, context.max_height);
        build_context_t child_context = {
            .max_width = context.max_width, .max_height = context.max_height,
            .min_width = context.min_width, .min_height = context.min_height,
            .backbuffer = backbuffers[i]
        };

        datas[i] = draw_component(child, (component_t*)layout, child_context, deepness + (num_childs - i));
    }

    coord_t top_left = { .x = context.max_width, .y = context.max_height };
    coord_t bottom_right = { .x = 0, .y = 0 };

    // render childs
    size_t ongoing_deepness = 0;
    const buffer_t* main_buffer = &context.backbuffer;
    for(uint64_t i = 0; i < num_childs; i++)
    {
        if(backbuffers[i].width == 0 || backbuffers[i].height == 0 || backbuffers[i].data == NULL)
            continue;

        coord_t child_top_left = datas[i].coords;
        coord_t child_bottom_right = {
            .x = child_top_left.x + datas[i].dims.width,
            .y = child_top_left.y + datas[i].dims.height
        };

        top_left.x = min(top_left.x, child_top_left.x);
        top_left.y = min(top_left.y, child_top_left.y);
        bottom_right.x = max(bottom_right.x, child_bottom_right.x);
        bottom_right.y = max(bottom_right.y, child_bottom_right.y);

        buffer_t* child_buffer = &backbuffers[i];
        uint64_t y_offset = datas[i].coords.y;
        uint64_t x_offset = datas[i].coords.x;

        size_t deepest_child = 0;
        if(datas[i].childs)
        {
            for(size_t c = 0; c < vector_size(datas[i].childs); c++)
            {
                widget_position_t child_pos = ((widget_position_t*)datas[i].childs)[c];
                child_pos.coords.x += x_offset;
                child_pos.coords.y += y_offset;
                vector_push_back(data.childs, child_pos);
                if(child_pos.z > deepest_child)
                    deepest_child = child_pos.z;
            }
            vector_destroy(datas[i].childs);
        }
        ongoing_deepness += deepest_child;

        widget_position_t child_pos = {
            .dimensions = datas[i].dims,
            .coords = datas[i].coords,
            .z = deepness + ongoing_deepness,
            .component = attributes->children->components[i]
        };
        vector_push_back(data.childs, child_pos);

        for (uint64_t y = 0; y < datas[i].dims.height; y++)
            for (uint64_t x = 0; x < datas[i].dims.width; x++)
            {
                color_t* main_pixel = &main_buffer->data[(y + y_offset) * main_buffer->width + (x + x_offset)]; 
                color_t* child_pixel = &child_buffer->data[(y + y_offset) * child_buffer->width + (x + x_offset)];
                *main_pixel = blend(*main_pixel, *child_pixel);
            }
    
        buffer_destroy(child_buffer);
    }

    data.dims.width = bottom_right.x - top_left.x;
    data.dims.height = bottom_right.y - top_left.y;
    data.coords = top_left;

    return data;
}

draw_data_t draw_flexible(const flexible_t* flexible, build_context_t context, uint64_t deepness)
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

draw_data_t draw_align(const align_t* align, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    const align_attributes_t* attributes = &(align->attributes);

    build_context_t child_context = {
        .max_height = context.max_height, .min_height = context.min_height,
        .max_width = context.max_width, .min_width = context.min_width,
        .backbuffer = buffer_create(context.max_width, context.max_height)
    };

    draw_data_t child_data = draw_component(attributes->child, (component_t*)align, child_context, deepness + 1);

    float align_x = (attributes->alignment->x + 1) / 2;
    float align_y = (attributes->alignment->y + 1) / 2;

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
        .component = attributes->child
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

draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context, uint64_t deepness)
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

draw_data_t draw_listener(const listener_t* listener, const build_context_t context, uint64_t deepness)
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

draw_data_t draw_text(const text_t* text, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    const text_attributes_t* attributes = &(text->attributes);

    buffer_t chars_buffer = buffer_create(context.max_width, context.max_height);

    for(int i = 0; i < wcslen(attributes->text); i++)
    {
        draw_data_t char_data = draw_character(&chars_buffer, 0xFFFFFFFF, attributes->text[i], data.dims.width, 0, *attributes->font_size);
        
        size_t char_width = data.dims.width - char_data.coords.x + char_data.dims.width;
        data.dims.width += char_width;
        
        size_t char_height = char_data.dims.height;
        data.dims.height = max(char_height, data.dims.height);
    }

    for(size_t x = 0; x < chars_buffer.width; x++)
        for(size_t y = 0; y < chars_buffer.height; y++)
        {
            color_t* bg_pixel = &context.backbuffer.data[y * context.backbuffer.width + x];
            color_t* char_pixel = &chars_buffer.data[y * chars_buffer.width + x];
            *bg_pixel = blend(*bg_pixel, *char_pixel);
        }

    buffer_destroy(&chars_buffer);

    if(data.coords.x + data.dims.width > context.max_width)
        data.dims.width = context.max_width - data.coords.x;
    if(data.coords.y + data.dims.height > context.max_height)
        data.dims.height = context.max_height - data.coords.y;

    return data;
}