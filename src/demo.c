#include <rund.h>
#include <rund/memory/gc.h>

#include <string.h>
#include <stdlib.h>

void key_press(component_t* component, uint16_t keycode)
{
    char buffer[4];
    if(keycode_to_ascii(keycode, buffer) == 0)
        return;

    text_t* text = (text_t*)rund_get_component("text");
    size_t l = strlen(text->attributes.text) + strlen(buffer);
    char* txt = gc_alloc(l);
    strcpy(txt, text->attributes.text);
    strcat(txt, buffer);
    text->attributes.text = txt;
}

void click(component_t* component)
{
    acquire_focus((listener_t*)rund_get_component("listener"));
}

int rund_main()
{
    rund_app_t app = {
        "Andrea",
        0, 0,
        1080 / 2, 720 / 2,
        Row(
            .children = list(
                Expanded(.child = Container()),
                Listener(
                    .id = "listener",
                    .handlers = Handlers(
                        .on_key_down = key_press,
                        .on_pointer_down = click,
                    ),
                    .child = Text(
                        .id = "text",
                        .text = "a",
                        .font_size = Val(size_t, 15)
                    ),
                ),
                Expanded(
                    .child = Container(
                        .decoration = ContainerDec(.color = 0xFFFFFF00)
                    )
                )
            ),
            .decoration = clone(&(row_decoration_t){.color = 0xFFFF00FF}, sizeof(row_decoration_t))
        )
    };

    run_app(&app);
}