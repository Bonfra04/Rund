#include <rund.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int rund_main()
{
    size_t some_value = 200;
    rund_app_t app = {
        "Andrea",
        0, 0,
        500, 500,
        Layout(
            .laying_style = Val((laying_style_t)Column),
            .children = list(
                Flexible(
                    .child = Text(
                        .text = L"Hello, World!"
                    ),
                ),
                Container(
                    .width = Val((size_t)200),
                    .height = Val((size_t)100),
                    .color = Val((color_t)0xFF0000FF),
                )
           ) 
        )
    };

    run_app(&app);
}