#include "leds.h"

void leds_task(void *Parameters)
{
	led_t *leds = (led_t *) Parameters;

	portTickType LastWake;

	gpio_set(leds->port, leds->pins);

	LastWake = xTaskGetTickCount();

	while(1) {
		gpio_toggle(leds->port, leds->pins);
		vTaskDelayUntil(&LastWake, leds->delay);
	}
}


