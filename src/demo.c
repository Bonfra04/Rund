#include <rund.h>

int rund_main()
{
    rund_app_t app = {
        "Andrea",
        0, 0,
        1080, 720 / 2,
        // Row(
        //     .children = list(
        //         Expanded(.child = Container()),
                Text(
                    .text = "beziercurves!96",
                    .font_size = Val(size_t, 20)
                ),
        //         Expanded(
        //             .child = Container(
        //                 .decoration = ContainerDec(.color = 0xFFFFFF00)
        //             )
        //         )
        //     ),
        //     .decoration = clone(&(row_decoration_t){.color = 0xFFFF00FF}, sizeof(row_decoration_t))
        // )
    };

    run_app(&app);
}