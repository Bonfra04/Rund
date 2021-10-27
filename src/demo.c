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
        Container(
            .decoration = ContainerDec(.color = 0xFFFF0000),
            .child = Align(
                .child = Text(
                    .id = "text",
                    .text = "aq0:",
                    .font_size = Val(size_t, 15)
                ),
                .alignment = Alignment(0.0, 0.0)
            )
        )
        
    };

    run_app(&app);
}