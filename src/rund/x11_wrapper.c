#ifdef __linux__

#include "rund.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

static Display* display;
static Window window;
static GC gc;

void create_window(const rund_app_t* app)
{
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