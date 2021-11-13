#include <rund.h>

#include "rund/wrapper.h"
#include "rund/renderer.h"
#include "rund/memory/internal_gc.h"

#include <string.h>
#include <wchar.h>
#include <time.h>

#define FPS_CAP 60l

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

static rund_app_t* app;
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
    while(component != NULL && !(component->flags & FLAG_LISTENER))
        component = component->parent;

    if(component == NULL || !(component->flags & FLAG_LISTENER))
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

static void on_resize(uint64_t width, uint64_t height)
{
    app->width = width;
    app->height = height;
}

static double frame_time()
{
    static clock_t start;
    clock_t end = clock();
    double cpu_used_time = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
    start = clock();
    return cpu_used_time;
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
        platform_msleep(1000l / FPS_CAP - (int)frame_time());

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
                put_pixel(x + data.coords.x, y + data.coords.y, context.backbuffer.data[(y + data.coords.y) * app->width + (x + data.coords.x)].argb);

        buffer_destroy(&context.backbuffer);

        flush();
    } while (event_loop());
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

void rund_acquire_focus(listener_t* listener)
{
    focus_node = listener;
}

draw_data_t draw_component(const component_t* component, const component_t* parent, const build_context_t context, uint64_t deepness)
{
    if(component)
    {
        component_t* c = (component_t*)component;
        c->parent = (component_t*)parent;

        if(c->draw_func)
            return c->draw_func(c, context, deepness);
    }

    return NULL_DRAW;
}