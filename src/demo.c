#include <rund.h>

static void click(component_t* self)
{
    container_t* red = (container_t*)rund_get_component("Red");
    red->attributes.decoration = ContainerDec(.color = 0xFF00FF);
}

int rund_main()
{
    rund_app_t app = {
        "Andrea",
        0, 0,
        1080 / 2, 720 / 2,
        Row(
            .children = list(
                Expanded(
                    .child = Container(
                        .id = "Red",
                        .width = Val(size_t, 100),
                        .height = Val(size_t, 100)
                    )
                ),
                ConstrainedBox(
                    .child = Container(
                        .width = Val(size_t, 10),
                        .height = Val(size_t, 10)
                    ),
                    .constraints = BoxConstraints(0, 0, 100, 100)
                ),
                Listener(
                    .child = Container(
                        .width = Val(size_t, 100),
                        .height = Val(size_t, 100),
                        .decoration = ContainerDec(.color = 0xFFFF00)
                    ),
                    .handlers = Handlers(.on_pointer_down = click)
                )
            )
        )
    };

    run_app(&app);
}