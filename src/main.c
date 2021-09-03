#include <rund.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

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
					Container(NULL, ContainerDec{ .color = 0x0000FF }),
					Flex(1)
				),
				Expanded(
					Container(NULL, ContainerDec{ .color = 0xFFFF00 }),
					Flex(2)
				),
				Expanded(
					Container(NULL, ContainerDec{ .color = 0xFFFFFF }),
					Flex(3)
				),
				Align(
					Container(NULL, 100, 100, ContainerDec{ .color = 0xFF006F }),
					Alignment(0.0f, 1.0f)
				),
				Container(NULL, 100, 100)
			),
			RowDec{ .color = 0xFF00FF}
		)
	};

	run_app(&app);
}