#include <rund.h>
#include <rund/memory/gc.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void click(component_t* self)
{
    printf("%s\n", self->id);
}

int rund_main()
{
    rund_app_t app = {
        "Andrea",
        0, 0,
        500, 500,
        Layout(
            .laying_style = Val(Stack),
            .children = list(
                Listener(
                    .id = "rosso",
                    .child = Align(
                        .alignment = Alignment(0, 0),
                        .child = Container(
                            .child = Flexible(
                                .child = Container(
                                    .decoration = ContainerDec(.color = 0xFFFF0000),
                                    .width = Val((size_t)100),
                                    .height = Val((size_t)100),
                                )
                            ),
                            .width = Val((size_t)200),
                            .height = Val((size_t)200),
                        ),
                    ),
                    .handlers = Handlers(.on_pointer_down = click)
                ),
                Listener(
                    .id = "testo",
                    .child = Align(
                        .child = Text(.text = L"AAAQ")
                    ),
                    .handlers = Handlers(.on_pointer_down = click)
                )
            )
        )
    };

    run_app(&app);
}