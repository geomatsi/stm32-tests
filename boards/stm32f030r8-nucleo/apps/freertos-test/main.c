#include <libopencm3/stm32/gpio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "FreeRTOSConfig.h"

#include "misc.h"
#include "init.h"
#include "leds.h"
#include "uart.h"

/* */

led_t led1 = { .port = GPIOA, .pins = GPIO5, .delay = 2000 };

/* */

int main(void)
{
	hw_init();
	printf("starting FreeRTOS...\n");

	xTaskCreate(leds_task, (const char *) "led", configMINIMAL_STACK_SIZE,
		(void *) &led1, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(uart_task, (const char *) "uart", configMINIMAL_STACK_SIZE,
		NULL, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	error_blink(2);
	return 0;
}
