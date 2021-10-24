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
                Expanded(.child = Container()),
                Text(
                    .text = "ga",
                    .font_size = Val(size_t, 15)
                ),
                Expanded(.child = Container())
            ),
            .decoration = clone(&(row_decoration_t){.color = 0xFFFF00}, sizeof(row_decoration_t))
        )
    };

    run_app(&app);
}