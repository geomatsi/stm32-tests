#ifndef LEDS_H
#define LEDS_H

#include <libopencm3/stm32/gpio.h>

#include "FreeRTOS.h"
#include "task.h"

/* */

typedef struct {
	uint32_t port;
	uint16_t pins;
	uint32_t ms;
} led_t;

/* */

void leds_task(void *Parameters);

#endif /* LEDS_H */
