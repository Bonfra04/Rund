#include <rund.h>
#include <rund/memory/gc.h>

#include <string.h>
#include <stdlib.h>


int rund_main()
{
    rund_app_t app = {
        "Andrea",
        0, 0,
        500, 500,
        Row(
            .children = list(
                Container(
                    .width = Val(size_t, 100),
                    .height = Val(size_t, 100),
                    .decoration = ContainerDec(.color = 0xFFFF0000),
                ),
                Text(
                    .text = L"Ciao",
                    .font_size = Val(float, 14),
                )
            )
        )
    };

    run_app(&app);
}