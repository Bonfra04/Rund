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
                Text(
                    .text = L"Ciao",
                    .font_size = Val(float, 14),
                ),
                Container(
                    .decoration = ContainerDec(.color = 0xFFFF0000),
                    .width = Val(size_t, 100),
                    .height = Val(size_t, 100),
                    .child =
                    Align(
                        .child = Text(
                            .text = L"Container",
                            .font_size = Val(float, 8),
                        ),
                        .alignment = Alignment(1.0, 1.0)
                    )
                ),
                Text(.text = L"EH"),
                Expanded(
                    .child = Container(
                        .decoration = ContainerDec(.color = 0xFF00FF00),
                    )
                )
            )
        )
    };

    run_app(&app);
}