#include "uart.h"

int putchar(int c)
{
	uint8_t ch = (uint8_t)c;

	usart_send_blocking(USART2, ch);
	return 0;
}

void uart_task(void *Parameters)
{
	TickType_t tick;

	while(1) {
		tick = xTaskGetTickCount();
		printf("tick(%u)\n", (unsigned int)tick);
		vTaskDelay(3000);
	}
}
