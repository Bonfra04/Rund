#include <rund.h>
#include <rund/memory/gc.h>

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
            .laying_style = Val((laying_style_t)Stack),
            .children = list(
                Align(
                    .child =  Container(
                        .width = Val((size_t)100),
                        .height = Val((size_t)100),
                        .color = Val((color_t)0xFFFF00FF),
                    )
                ),
                Container(
                    .width = Val((size_t)100),
                    .height = Val((size_t)100),
                    .color = Val((color_t)0xFF0000FF),
                )
           ) 
        )
    };

    run_app(&app);
}