#include <libopencm3/stm32/gpio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "FreeRTOSConfig.h"

#include "misc.h"
#include "init.h"
#include "button.h"
#include "leds.h"

/* */

led_t led1 = { .port = GPIOA, .pins = GPIO5, .delay = 2000 };

/* */

int main(void)
{
	hw_init();

	button_init();

	xTaskCreate(leds_task, (const char *) "led1", configMINIMAL_STACK_SIZE, (void *) &led1, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(button_task, (const char *) "button", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	error_blink(GPIOA, GPIO5);
	return 0;
}
