#include <libopencm3/stm32/usart.h>

int putchar(int c)
{
	uint16_t ch = (uint16_t) c;
	usart_send_blocking(USART2, ch);
	return 0;
}
