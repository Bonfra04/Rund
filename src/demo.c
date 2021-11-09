#include <rund.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void click(component_t* self)
{
    printf("HEH\n");
}

int rund_main()
{
    size_t some_value = 200;
    rund_app_t app = {
        "Andrea",
        0, 0,
        500, 500,
        Layout(
            .laying_style = Val((laying_style_t)Row),
            .children = list(
                Flexible(
                    .child = Text(
                        .text = L"Hello, World!"
                    ),
                    .flex = Val((uint64_t)1) // this flexible will occupy 1/3 of the space
                ),
                Container(
                    .width = Val((size_t)200),
                    .height = Val((size_t)100),
                    .color = Val((color_t)0xFF0000FF),
                ),
                Flexible(
                    .child = Text(
                        .text = L"Hello, World!"
                    ),
                    .flex = Val((uint64_t)2) // this flexible will occupy 2/3 of the space
                )
            ) 
        )
    };

    run_app(&app);
}