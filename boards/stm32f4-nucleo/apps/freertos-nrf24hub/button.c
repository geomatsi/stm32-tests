#include "button.h"

/* */

QueueHandle_t xQueue;
bool exti_falling;

/* */

void exti15_10_isr(void)
{
	unsigned char msg;

	exti_reset_request(EXTI13);

	if (exti_falling) {
		msg = 'P';
		xQueueSendToBackFromISR(xQueue, &msg, NULL);
		exti_falling = false;
		exti_set_trigger(EXTI13, EXTI_TRIGGER_RISING);
	} else {
		msg = 'R';
		xQueueSendToBackFromISR(xQueue, &msg, NULL);
		exti_falling = true;
		exti_set_trigger(EXTI13, EXTI_TRIGGER_FALLING);
	}
}

void button_init(void)
{
	xQueue = xQueueCreate( 16, sizeof( unsigned char ) );

	if (!xQueue) {
		printf("ERROR: can't create queue\n\r");
	}

	exti_falling = true;
}

void button_task(void *Parameters)
{
	unsigned char c;

	printf("started button task...\n\r");

	while(1) {
		xQueueReceive(xQueue, &c, portMAX_DELAY);
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
