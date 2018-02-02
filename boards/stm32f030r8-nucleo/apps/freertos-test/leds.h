#ifndef _LEDS_H_
#define _LEDS_H_

#include <libopencm3/stm32/gpio.h>

#include "FreeRTOS.h"
#include "task.h"

/* */

typedef struct {
	uint32_t port;
	uint16_t pins;
	uint32_t delay;
} led_t;

/* */

void leds_task(void *Parameters);

#endif /* _LEDS_H_ */
