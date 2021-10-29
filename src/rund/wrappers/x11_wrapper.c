#ifdef __linux__

#include <rund/wrapper.h>
#include <rund/keycodes.h>

#include <X11/Xlib.h>
#include <GL/glx.h>

static Display* display;
static Window window;
static GC gc;

static events_t handlers;

static uint16_t translate_keycode(uint32_t x11_keycode);

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

            case KeyPress:
                if(handlers.key_down)
                    handlers.key_down(translate_keycode(event.xkey.keycode));
                break;

            case KeyRelease:
                if(handlers.key_up)
                    handlers.key_up(translate_keycode(event.xkey.keycode));
                break;

            case ConfigureNotify:
                if(handlers.resize)
                    handlers.resize(event.xconfigure.width, event.xconfigure.height);
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

#include <stdio.h>
#include <stdlib.h>

static uint16_t translate_keycode(uint32_t x11_keycode)
{
    KeySym keysym = XKeycodeToKeysym(display, x11_keycode, 0);
    switch (keysym)
    {
    case XK_A: return RK_A;
    case XK_B: return RK_B;
    case XK_C: return RK_C;
    case XK_D: return RK_D;
    case XK_E: return RK_E;
    case XK_F: return RK_F;
    case XK_G: return RK_G;
    case XK_H: return RK_H;
    case XK_I: return RK_I;
    case XK_J: return RK_J;
    case XK_K: return RK_K;
    case XK_L: return RK_L;
    case XK_M: return RK_M;
    case XK_N: return RK_N;
    case XK_O: return RK_O;
    case XK_P: return RK_P;
    case XK_Q: return RK_Q;
    case XK_R: return RK_R;
    case XK_S: return RK_S;
    case XK_T: return RK_T;
    case XK_U: return RK_U;
    case XK_V: return RK_V;
    case XK_W: return RK_W;
    case XK_X: return RK_X;
    case XK_Y: return RK_Y;
    case XK_Z: return RK_Z;
    case XK_a: return RK_a;
    case XK_b: return RK_b;
    case XK_c: return RK_c;
    case XK_d: return RK_d;
    case XK_e: return RK_e;
    case XK_f: return RK_f;
    case XK_g: return RK_g;
    case XK_h: return RK_h;
    case XK_i: return RK_i;
    case XK_j: return RK_j;
    case XK_k: return RK_k;
    case XK_l: return RK_l;
    case XK_m: return RK_m;
    case XK_n: return RK_n;
    case XK_o: return RK_o;
    case XK_p: return RK_p;
    case XK_q: return RK_q;
    case XK_r: return RK_r;
    case XK_s: return RK_s;
    case XK_t: return RK_t;
    case XK_u: return RK_u;
    case XK_v: return RK_v;
    case XK_w: return RK_w;
    case XK_x: return RK_x;
    case XK_y: return RK_y;
    case XK_z: return RK_z;
    case XK_0: return RK_0;
    case XK_1: return RK_1;
    case XK_2: return RK_2;
    case XK_3: return RK_3;
    case XK_4: return RK_4;
    case XK_5: return RK_5;
    case XK_6: return RK_6;
    case XK_7: return RK_7;
    case XK_8: return RK_8;
    case XK_9: return RK_9;
    case XK_space: return RK_space;
    }

    fprintf(stderr, "Unmapped keycode: %d\n", x11_keycode);
    exit(-1);
}

#endif