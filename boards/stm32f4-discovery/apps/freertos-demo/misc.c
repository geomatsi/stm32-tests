#include "misc.h"

/* */

static void xdelay(volatile uint32_t count)
{
	while(count) {
		count--;
	}
}

/* */

void error_blink(uint32_t port, uint16_t pins)
{
	gpio_clear(port, pins);

	while (1) {
		gpio_toggle(port, pins);
		xdelay(0x3fffff);
	}

	gpio_clear(port, pins);
}

void test_blink(uint32_t port, uint16_t pins, uint32_t num)
{
	gpio_clear(port, pins);

	while (num--) {
		gpio_toggle(port, pins);
		xdelay(0x3fffff);
	}

	gpio_clear(port, pins);
}
