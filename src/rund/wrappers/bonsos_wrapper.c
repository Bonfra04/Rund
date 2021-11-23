#ifdef BONSOS

#include "../wrapper.h"
#include <gui.h>

static window_t window;

static void event_handler(uint64_t wnd_id, uint8_t event_id)
{
    switch(event_id)
    {
        case WND_EVENT_KEY_DOWN:
            printf("CIAO");
            break;

        default:
            default_event_handler(wnd_id, event_id);
            break;
    }
}

void create_window(const rund_app_t* app, events_t handlers)
{
    window = window_create(app->title, app->width, app->height, app->x, app->y, WND_NO_DECORATIONS);
}

bool event_loop()
{
    return cycle_events(event_handler);
}

void put_pixel(size_t x, size_t y, uint32_t color)
{
    ((uint32_t*)window.framebuffer)[y * window.width + x] = 0xFF000000 | (color & 0xFFFFFF);
}

void flush()
{
    // nothing to do here
}

#endif