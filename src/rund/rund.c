#include <rund.h>
#include <rund/core.h>
#include <rund/wrapper.h>
#include <rund/utils/vector.h>
#include <rund/log.h>
#include <rund/renderer.h>

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
draw_data_t draw_row(const row_t* row, const build_context_t context, uint64_t deepness);
draw_data_t draw_column(const column_t* column, const build_context_t context, uint64_t deepness);
draw_data_t draw_expanded(const expanded_t* expanded, const build_context_t context, uint64_t deepness);
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

        draw_data_t data = draw_component(app->root, NULL, context, 0);
        widget_position_t root_pos = {
            .dimensions = data.dimensions,
            .coords = data.coords,
            .z = 0,
            .component = app->root
        };

        widgets = data.childs ?: vector_create(widget_position_t);
        vector_push_back(widgets, root_pos);

        for(uint64_t y = 0; y < app->height; y++)
            for(uint64_t x = 0; x < app->width; x++)
                put_pixel(x, y, context.backbuffer.data[y * app->width + x]);

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
        case ROW:
            data = draw_row((const row_t*)component, context, deepness);
            break;
        case COLUMN:
            data = draw_column((const column_t*)component, context, deepness);
            break;
        case EXPANDED:
            data = draw_expanded((const expanded_t*)component, context, deepness);
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

    size_t max_width = context.max_width;
    size_t max_height = context.max_height;
    if(attributes->width)
        max_width = *attributes->width > context.max_width ? context.max_width : *attributes->width;
    if(attributes->height)
        max_height = *attributes->height > context.max_height ? context.max_height : *attributes->height;

    data.dimensions.height = max_height;
    data.dimensions.width = max_width;

    for (uint64_t y = 0; y < data.dimensions.height; y++)
        for (uint64_t x = 0; x < data.dimensions.width; x++)
            BLEND(context.backbuffer.data[y *context.backbuffer.width + x], attributes->decoration->color);

    if(attributes->child)
    {
        build_context_t child_context = {
            .max_width = max_width, .max_height = max_height,
            .min_width = context.min_width, .min_height = context.min_height,
            .backbuffer = buffer_create(max_width, max_height)
        };

        draw_data_t child_data = draw_component(attributes->child, (component_t*)container, child_context, deepness + 1);

        data.childs = child_data.childs;

        child_data.dimensions.width = child_data.dimensions.width > max_width ? max_width : child_data.dimensions.width;
        child_data.dimensions.height = child_data.dimensions.height > max_height ? max_height : child_data.dimensions.height;

        if(!attributes->width)
            data.dimensions.width = child_data.dimensions.width;
        if(!attributes->height)
            data.dimensions.height = child_data.dimensions.height;

        widget_position_t child_pos = {
            .dimensions = child_data.dimensions,
            .coords = child_data.coords,
            .z = deepness,
            .component = attributes->child
        };
        if(!data.childs)
            data.childs = vector_create(widget_position_t);
        vector_push_back(data.childs, child_pos);

        for(size_t x = 0; x < child_data.dimensions.width; x++)
            for(size_t y = 0; y < child_data.dimensions.height; y++)
            {
                color_t* child_pixel = &child_context.backbuffer.data[(child_data.coords.y + y) * child_context.backbuffer.width + (child_data.coords.x + x)];
                color_t* parent_pixel = &context.backbuffer.data[(child_data.coords.y + y) * context.max_width + (child_data.coords.x + x)];
                BLEND(*parent_pixel, *child_pixel);
            }

        buffer_destroy(&child_context.backbuffer);
    }

    return data;
}

draw_data_t draw_row(const row_t* row, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = { .coords = { 0, 0 }, .dimensions = { context.max_width, context.max_height } };
    data.childs = vector_create(widget_position_t);

    const row_attributes_t* attributes = &(row->attributes);

    // decorations
    for(uint64_t y = 0; y < context.max_height; y++)
        for(uint64_t x = 0; x < context.max_width; x++)
            BLEND(context.backbuffer.data[y * context.backbuffer.width + x], attributes->decoration->color);

    buffer_t backbuffers[attributes->children->length];
    draw_data_t datas[attributes->children->length];

    uint64_t unflexible_width = 0;
    uint64_t flexible_count = 0;

    // draw unflexibles
    for(uint64_t i = 0; i < attributes->children->length; i++)
    {
        component_t* child = attributes->children->components[i];
        if(child->isFlexible)
        {
            flexible_count += *((flexible_t*)child)->flex;
            continue;
        }

        build_context_t child_context = context;
        child_context.max_width -= unflexible_width;
        backbuffers[i] = buffer_create(child_context.max_width, child_context.max_height);
        child_context.backbuffer = backbuffers[i];

        datas[i] = draw_component(child, (component_t*)row, child_context, deepness + 1);

        unflexible_width += datas[i].dimensions.width;
    }

    uint64_t left_width = context.max_width > unflexible_width ? context.max_width - unflexible_width : 0;

    // draw flexibles
    for(uint64_t i = 0; i < attributes->children->length; i++)
    {
        flexible_t* child = (flexible_t*)attributes->children->components[i];
        if(!child->isFlexible)
            continue;

        dimension_t space;
        space.width = (uint64_t)(((double)left_width * (double)*child->flex) / (double)flexible_count + 0.5);
        space.height = context.max_height;
        backbuffers[i] = buffer_create(space.width, space.height);

        build_context_t child_context = context;
        child_context.max_width = space.width;
        child_context.max_height = space.height;
        child_context.backbuffer = backbuffers[i];

        datas[i] = draw_component((component_t*)child, (component_t*)row, child_context, deepness + 1);
    }

    uint64_t advance = 0;

    // render childs
    const buffer_t* main_buffer = &context.backbuffer;
    for(uint64_t i = 0; i < attributes->children->length; i++)
    {
        buffer_t* child_buffer = &backbuffers[i];
        uint64_t y_offset = datas[i].coords.y;
        uint64_t x_offset = datas[i].coords.x;

        widget_position_t position;
        position.dimensions = datas[i].dimensions;
        position.coords = (coord_t){ advance, y_offset };
        position.component = attributes->children->components[i];
        position.z = deepness + 1;
        vector_push_back(data.childs, position);

        if(datas[i].childs)
        {
            for(size_t c = 0; c < vector_size(datas[i].childs); c++)
            {
                widget_position_t child_position = ((widget_position_t*)datas[i].childs)[c];
                child_position.coords.x += advance - x_offset;
                child_position.coords.y = y_offset;
                vector_push_back(data.childs, child_position);
            }
            vector_destroy(datas[i].childs);
        }

        for (uint64_t y = 0; y < datas[i].dimensions.height; y++)
            for (uint64_t x = 0; x < datas[i].dimensions.width; x++)
            {
                color_t* main_pixel = &main_buffer->data[(y + y_offset) * main_buffer->width + (x + advance)]; 
                color_t* child_pixel = &child_buffer->data[(y + y_offset) * child_buffer->width + (x + x_offset)];
                BLEND(*main_pixel, *child_pixel);
            }

        advance += datas[i].dimensions.width;
    
        buffer_destroy(child_buffer);
    }

    return data;
}

draw_data_t draw_column(const column_t* column, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = { .coords = { 0, 0 }, .dimensions = { context.max_width, context.max_height } };
    data.childs = vector_create(widget_position_t);

    const column_attributes_t* attributes = &(column->attributes);

    // decorations
    for(uint64_t y = 0; y < context.max_height; y++)
        for(uint64_t x = 0; x < context.max_width; x++)
            BLEND(context.backbuffer.data[y * context.backbuffer.width + x], attributes->decoration->color);

    buffer_t backbuffers[attributes->children->length];
    draw_data_t datas[attributes->children->length];

    uint64_t unflexible_height = 0;
    uint64_t flexible_count = 0;

    // draw unflexibles
    for(uint64_t i = 0; i < attributes->children->length; i++)
    {
        component_t* child = attributes->children->components[i];
        if(child->isFlexible)
        {
            flexible_count += *((flexible_t*)child)->flex;
            continue;
        }

        build_context_t child_context = context;
        child_context.max_height -= unflexible_height;
        backbuffers[i] = buffer_create(child_context.max_width, child_context.max_height);
        child_context.backbuffer = backbuffers[i];

        datas[i] = draw_component(child, (component_t*)column, child_context, deepness + 1);

        unflexible_height += datas[i].dimensions.height;
    }

    uint64_t left_height = context.max_height > unflexible_height ? context.max_height - unflexible_height : 0;

    // draw flexibles
    for(uint64_t i = 0; i < attributes->children->length; i++)
    {
        flexible_t* child = (flexible_t*)attributes->children->components[i];
        if(!child->isFlexible)
            continue;

        dimension_t space;
        space.height = (uint64_t)(((double)left_height * (double)*child->flex) / (double)flexible_count + 0.5);
        space.width = context.max_width;
        backbuffers[i] = buffer_create(space.width, space.height);

        build_context_t child_context = context;
        child_context.max_width = space.width;
        child_context.max_height = space.height;
        child_context.backbuffer = backbuffers[i];

        datas[i] = draw_component((component_t*)child, (component_t*)column, child_context, deepness + 1);
    }

    uint64_t advance = 0;

    // render childs
    const buffer_t* main_buffer = &context.backbuffer;
    for(uint64_t i = 0; i < attributes->children->length; i++)
    {
        buffer_t* child_buffer = &backbuffers[i];
        uint64_t y_offset = datas[i].coords.y;
        uint64_t x_offset = datas[i].coords.x;

        widget_position_t position;
        position.dimensions = datas[i].dimensions;
        position.coords = (coord_t){ x_offset, advance };
        position.component = attributes->children->components[i];
        position.z = deepness + 1;
        vector_push_back(data.childs, position);

        if(datas[i].childs)
        {
            for(size_t c = 0; c < vector_size(datas[i].childs); c++)
            {
                widget_position_t child_position = ((widget_position_t*)datas[i].childs)[c];
                child_position.coords.x = x_offset;
                child_position.coords.y += advance - y_offset;
                vector_push_back(data.childs, child_position);
            }
            vector_destroy(datas[i].childs);
        }

        for (uint64_t y = 0; y < datas[i].dimensions.height; y++)
            for (uint64_t x = 0; x < datas[i].dimensions.width; x++)
            {
                color_t* main_pixel = &main_buffer->data[(y + advance) * main_buffer->width + (x + x_offset)]; 
                color_t* child_pixel = &child_buffer->data[(y + y_offset) * child_buffer->width + (x + x_offset)];
                BLEND(*main_pixel, *child_pixel);
            }

        advance += datas[i].dimensions.height;
    
        buffer_destroy(child_buffer);
    }

    return data;
}

draw_data_t draw_expanded(const expanded_t* expanded, build_context_t context, uint64_t deepness)
{
    build_context_t child_context = context;
    child_context.min_height = child_context.max_height;
    child_context.min_width = child_context.max_width;

    const expanded_attributes_t* attributes = &expanded->attributes;

    draw_data_t data = draw_component(attributes->child, (component_t*)expanded, child_context, deepness + 1);

    if(data.childs == NULL)
        data.childs = vector_create(widget_position_t);

    widget_position_t child_pos = {
        .dimensions = data.dimensions,
        .coords = data.coords,
        .z = deepness + 1,
        .component = attributes->child
    };

    vector_push_back(data.childs, child_pos);

    return data;
}

draw_data_t draw_align(const align_t* align, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = NULL_DRAW;

    buffer_t backbuffer = buffer_create(context.max_width, context.max_height);

    build_context_t child_context = context;
    child_context.backbuffer = backbuffer;

    const align_attributes_t* attributes = &(align->attributes);

    draw_data_t child_data = draw_component(attributes->child, (component_t*)align, child_context, deepness + 1);

    float align_x = (attributes->alignment->x + 1) / 2;
    float align_y = (attributes->alignment->y + 1) / 2;

    uint64_t xp = context.max_width * align_x;
    uint64_t yp = context.max_height * align_y;

    uint64_t xc = child_data.dimensions.width * align_x;
    uint64_t yc = child_data.dimensions.height * align_y;

    const buffer_t* main_buffer = &context.backbuffer;
    uint64_t y_offset = yp - yc;
    uint64_t x_offset = xp - xc;

    for (uint64_t y = 0; y < child_data.dimensions.height; y++)
        for (uint64_t x = 0; x < child_data.dimensions.width; x++)
        {
            color_t* main_pixel = &main_buffer->data[(y_offset + y) * main_buffer->width + (x_offset + x)];
            color_t* child_pixel = &backbuffer.data[y * backbuffer.width + x];
            *main_pixel = *child_pixel;
        }

    buffer_destroy(&backbuffer);

    data.dimensions = child_data.dimensions;
    data.coords = (coord_t){ (xp - xc), (yp - yc) };
    data.childs = child_data.childs;

    if(data.childs == NULL)
        data.childs = vector_create(widget_position_t);

    widget_position_t child_pos = {
        .dimensions = data.dimensions,
        .coords = data.coords,
        .z = deepness + 1,
        .component = attributes->child
    };

    vector_push_back(data.childs, child_pos);

    return data;
}

draw_data_t draw_constrained_box(const constrained_box_t* constrained_box, const build_context_t context, uint64_t deepness)
{
    build_context_t child_context = context;

    const constrained_box_attributes_t* attributes = &(constrained_box->attributes);

    child_context.min_height = context.min_height > attributes->constraints->min_height ? context.min_height : attributes->constraints->min_height;
    child_context.min_width = context.min_width > attributes->constraints->min_width ? context.min_width : attributes->constraints->min_width;
    child_context.max_height = context.max_height < attributes->constraints->max_height ? context.max_height : attributes->constraints->max_height;
    child_context.max_width = context.max_width < attributes->constraints->max_width ? context.max_width : attributes->constraints->max_width;
    
    draw_data_t data = draw_component(attributes->child, (component_t*)constrained_box, child_context, deepness + 1);

    if(data.childs == NULL)
        data.childs = vector_create(widget_position_t);

    widget_position_t child_pos = {
        .dimensions = data.dimensions,
        .coords = data.coords,
        .z = deepness + 1,
        .component = attributes->child
    };

    vector_push_back(data.childs, child_pos);

    return data;
}

draw_data_t draw_listener(const listener_t* listener, const build_context_t context, uint64_t deepness)
{
    const listener_attributes_t* attributes = &(listener->attributes);

    draw_data_t data = draw_component(attributes->child, (component_t*)listener, context, deepness + 1);

    if(data.childs == NULL)
        data.childs = vector_create(widget_position_t);

    widget_position_t child_pos = {
        .dimensions = data.dimensions,
        .coords = data.coords,
        .z = deepness + 1,
        .component = attributes->child
    };

    vector_push_back(data.childs, child_pos);

    return data;
}

draw_data_t draw_text(const text_t* text, const build_context_t context, uint64_t deepness)
{
    draw_data_t data = { .childs = NULL, .dimensions = { 0, 0 }, .coords = { 0, 0 } };

    const text_attributes_t* attributes = &(text->attributes);

    buffer_t chars_buffer = buffer_create(context.max_width, context.max_height);

    for(int i = 0; i < wcslen(attributes->text); i++)
    {
        draw_data_t char_data = draw_character(&chars_buffer, 0xFFFFFFFF, attributes->text[i], data.dimensions.width, 0, *attributes->font_size);
        
        size_t char_width = data.dimensions.width - char_data.coords.x + char_data.dimensions.width;
        data.dimensions.width += char_width;
        
        size_t char_height = char_data.dimensions.height;
        data.dimensions.height = char_height > data.dimensions.height ? char_height : data.dimensions.height;
    }

    for(size_t x = 0; x < chars_buffer.width; x++)
        for(size_t y = 0; y < chars_buffer.height; y++)
            BLEND(context.backbuffer.data[y * context.backbuffer.width + x], chars_buffer.data[y * chars_buffer.width + x]);

    buffer_destroy(&chars_buffer);

    if(data.coords.x + data.dimensions.width > context.max_width)
        data.dimensions.width = context.max_width - data.coords.x;
    if(data.coords.y + data.dimensions.height > context.max_height)
        data.dimensions.height = context.max_height - data.coords.y;

    return data;
}