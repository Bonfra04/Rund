#include <rund.h>
#include <rund/memory/gc.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void click(component_t* self)
{
    listener_t* listener = (listener_t*)rund_get_component("listener");
    rund_acquire_focus(listener);
}

void type(component_t* self, uint16_t keycode)
{
    wchar_t ch = keycode_to_unicode(keycode);
    if(ch != 0)
    {
        text_t* text = (text_t*)rund_get_component("text");
        size_t old_len = wcslen(text->attributes.text);
        wchar_t* new_text = gc_alloc(sizeof(wchar_t) * (old_len + 2));
        wcscpy(new_text, text->attributes.text);
        new_text[old_len + 0] = ch;
        new_text[old_len + 1] = L'\0';
        text->attributes.text = new_text;
    }
}

int rund_main()
{
    rund_app_t app = {
        "Andrea",
        0, 0,
        500, 500,
        Listener(
            .id = "listener",
            .child = Container(
                .child = Text(
                    .id = "text",
                    .text = L""
                ),
                .width = Val((size_t)300),
                .height = Val((size_t)100),
                .color = Val(Color(0xFFaaaaaa))
            ),
            .handlers = Handlers(
                .on_pointer_down = click,
                .on_key_down = type
            )
        )
    };

    run_app(&app);
}