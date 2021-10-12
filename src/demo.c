#include <rund.h>
#include <rund/utils/allocation.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void click(component_t* self)
{
    printf("%p\n", self);
}

int main()
{
    rund_app_t app = {
        "Andrea",
        0, 0,
        1080 / 2, 720 / 2,
        Row(
            .children = clone(&list(
                Expanded(
                    .child = Container(
                        .width = alloc_size(100),
                        .height = alloc_size(100)
                    ),
                ),
                Listener(
                    .child = Container(
                        .width = alloc_size(100),
                        .height = alloc_size(100),
                        .decoration = clone(&ContainerDec(.color = 0xFFFF00), sizeof(container_decoration_t))
                    ),
                    .handlers = clone(&Handlers(.on_pointer_down = click), sizeof(handlers_t))
                ),
                Expanded(
                    .child = Container(
                        .width = alloc_size(100),
                        .height = alloc_size(100),
                        .decoration = clone(&ContainerDec(.color = 0xFFFFFF), sizeof(container_decoration_t))
                    ),
                )
            ), sizeof(component_list_t))
        )
    };

    run_app(&app);
}