#ifdef __linux__

#include "../wrapper.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

static Display* display;
static Window window;
static GC gc;

static events_t handlers;

void create_window(const rund_app_t* app, events_t _handlers)
{
    handlers = _handlers;

    display = XOpenDisplay(NULL);
    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
        app->x, app->y,
        app->width, app->height,
        1,
        BlackPixel(display, screen),
        BlackPixel(display, screen)
    );

    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(display, window, &wmDelete, 1);

    XSelectInput(display, window,
        ExposureMask | KeyPressMask |
        KeyReleaseMask | ButtonPressMask |
        ButtonReleaseMask | PointerMotionMask |
        FocusChangeMask | StructureNotifyMask
    );

    XMapWindow(display, window);
    XStoreName(display, window, app->title);    

    gc = XCreateGC(display, window, 0, NULL);
}

bool event_loop()
{
    XEvent event;
    while(XPending(display))
    {
        XNextEvent(display, &event);

        switch(event.type)
        {
            case ClientMessage:
                return false;

            case ButtonPress:
                if(handlers.mouse_down)
                    handlers.mouse_down(event.xbutton.x, event.xbutton.y, event.xbutton.button);
                break;

            case ButtonRelease:
                if(handlers.mouse_up)
                    handlers.mouse_up(event.xbutton.x, event.xbutton.y, event.xbutton.button);
                break;

            default:
                break;
        }
    }

    return true;
}

void put_pixel(size_t x, size_t y, uint32_t color)
{
    XSetForeground(display, gc, color);
    XDrawPoint(display, window, gc, x, y);
}

void flush()
{
    XFlush(display);
}

#endif