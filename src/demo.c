#include <rund.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void click()
{
    printf("Hello there\n");
}

int main()
{
    rund_app_t app = {
        "Andrea",
        0, 0,
        1080 / 2, 720 / 2,
        Row(
            list(
                Container(NULL, 100, 100, ContainerDec{ .color = 0x00FF00 }),
                Expanded(
                    Container(NULL, ContainerDec{ .color = 0xFFFF00 }),
                    Flex(2)
                ),
                Expanded(
                    Container(NULL, ContainerDec{ .color = 0xFFFFFF }),
                    Flex(1)
                ),
                Align(
                    Container(NULL, 100, 100, ContainerDec{ .color = 0xFF006F }),
                    Alignment(0.0f, 1.0f)
                ),
                Listener(
                    ConstrainedBox(
                        Container(NULL, 1, 1),
                        BoxConstraints(10, 10, 30, 30)
                    ),
                    (Handlers{ .on_pointer_up = NULL, .on_pointer_down = click })
                )
            )
        )
    };

    run_app(&app);
}