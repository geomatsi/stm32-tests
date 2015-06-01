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

led_t led1 = { .port = GPIOD, .pins = GPIO12, .delay =  500 };
led_t led2 = { .port = GPIOD, .pins = GPIO13, .delay = 1000 };
led_t led3 = { .port = GPIOD, .pins = GPIO14, .delay = 1500 };

/* */

int main(void)
{
	hw_init();

	button_init();

	xTaskCreate(leds_task, (const char *) "led1", configMINIMAL_STACK_SIZE, (void *) &led1, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(leds_task, (const char *) "led2", configMINIMAL_STACK_SIZE, (void *) &led2, tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(leds_task, (const char *) "led3", configMINIMAL_STACK_SIZE, (void *) &led3, tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(button_task, (const char *) "button", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	error_blink(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
	return 0;
}
