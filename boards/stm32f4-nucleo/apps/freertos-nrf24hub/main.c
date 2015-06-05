#include <libopencm3/stm32/gpio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "FreeRTOSConfig.h"

#include "misc.h"
#include "init.h"
#include "button.h"
#include "leds.h"
#include "radio.h"

/* */

led_t led = { .port = GPIOB, .pins = GPIO10, .delay = 1000 };

/* */

int main(void)
{
	hw_init();

	button_init();

	xTaskCreate(leds_task, (const char *) "led", configMINIMAL_STACK_SIZE, (void *) &led, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(button_task, (const char *) "button", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(radio_task, (const char *) "radio", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

	printf("TP#4\n\r");

	vTaskStartScheduler();

	error_blink(GPIOB, GPIO10);
	return 0;
}
