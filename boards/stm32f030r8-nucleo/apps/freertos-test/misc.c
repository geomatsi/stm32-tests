#include "misc.h"

/* */

static void xdelay(volatile uint32_t count)
{
	while(count) {
		count--;
	}
}

/* */

void error_blink(int val)
{
	gpio_clear(GPIOA, GPIO5);

	while (1) {
		gpio_toggle(GPIOA, GPIO5);
		xdelay(0x1ffff * val);
	}
}

void assert_blink(void)
{
	gpio_clear(GPIOA, GPIO5);

	while (1) {
		gpio_toggle(GPIOA, GPIO5);
		xdelay(0x1ffff);
	}
}
