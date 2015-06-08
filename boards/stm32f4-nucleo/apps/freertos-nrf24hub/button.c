#include "button.h"

/* */

QueueHandle_t xButtonQueue;
bool exti_falling;

/* */

void button_init(void)
{
	xButtonQueue = xQueueCreate( 16, sizeof( unsigned char ) );

	if (!xButtonQueue) {
		printf("ERROR: can't create button queue\n\r");
	}

	exti_falling = true;
}

void button_task(void *Parameters)
{
	unsigned char c;

	printf("started button task...\n\r");

	while(1) {
		xQueueReceive(xButtonQueue, &c, portMAX_DELAY);
		printf("button_task got %c message\n\r", c);

		switch (c) {
			case 'P':
				printf("button pressed\n\r");
				break;
			case 'R':
				printf("button released\n\r");
				break;
			default:
				break;
		}
	}
}
