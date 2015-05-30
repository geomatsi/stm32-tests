#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>

#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "clock.h"

/* */

uint32_t SystemCoreClock;
bool exti0_falling;
xQueueHandle xQueue;

/* */

void xdelay(volatile uint32_t count)
{
	while(count) {
		count--;
	}
}

static void critical_error_blink(void)
{
	gpio_clear(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	while (1) {
		gpio_toggle(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
		xdelay(0x3fffff);
	}
}

static void test_blink(uint32_t pin, uint32_t bank, uint32_t num)
{
	gpio_clear(bank, pin);

	while (num--) {
		gpio_toggle(bank, pin);
		xdelay(0x3fffff);
	}

	gpio_clear(bank, pin);
}

/* */

void exti0_isr(void)
{
	unsigned char msg;

	exti_reset_request(EXTI0);

	if (exti0_falling) {
		msg = 'D';
		xQueueSendToFrontFromISR(xQueue, &msg, NULL);
		exti0_falling = false;
		exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);
	} else {
		msg = 'E';
		xQueueSendToFrontFromISR(xQueue, &msg, NULL);
		exti0_falling = true;
		exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);
	}
}

/* */

static void hw_init(void)
{
	/* TODO: sync SystemCoreClock with rcc_clock_setup_xxx settings */
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);
	SystemCoreClock = 120000000;

	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOA);

	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);

	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);

	gpio_clear(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	/* enable EXTI0 interrupt */

	nvic_enable_irq(NVIC_EXTI0_IRQ);

	/* configure the EXTI subsystem */

	exti_select_source(EXTI0, GPIOA);
	exti0_falling = false;
	exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);
	exti_enable_request(EXTI0);

	/*
		http://www.freertos.org/RTOS-Cortex-M3-M4.html
		Preempt priority and subpriority:

			If you are using an STM32 with the STM32 driver library then ensure all the
			priority bits are assigned to be preempt priority bits by calling
			NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 ); before the RTOS is started.
	*/

	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP16_NOSUB);

	/* DON'T PUT ANYTHING HERE AFTER NVIC PRIO GROUP ARE PROPERLY CONFIGURED FOR FREERTOS */
}

/* */

static void ButtonControl(void *Parameters)
{
	unsigned char c;

	gpio_clear(GPIOD, GPIO14);

	while(1) {
		xQueueReceive(xQueue, &c, 10000);

		switch (c) {
			case 'E':
				gpio_set(GPIOD, GPIO14);
				break;
			case 'D':
				gpio_clear(GPIOD, GPIO14);
				break;
			default:
				break;
		}
	}
}

static void LedFlash1(void *Parameters)
{
	uint32_t delay = (uint32_t) Parameters;

	portTickType LastWake;

	gpio_set(GPIOD, GPIO12);

	LastWake = xTaskGetTickCount();

	while(1) {
		gpio_toggle(GPIOD, GPIO12);
		vTaskDelayUntil(&LastWake, delay);
	}
}

static void LedFlash2(void *Parameters)
{
	uint32_t delay = (uint32_t) Parameters;

	portTickType LastWake;

	gpio_set(GPIOD, GPIO13 | GPIO15);

	LastWake = xTaskGetTickCount();

	while(1) {
		gpio_toggle(GPIOD, GPIO13 | GPIO15);
		vTaskDelayUntil(&LastWake, delay);
	}
}

int main(void)
{
	hw_init();

	xTaskCreate(LedFlash1, (signed char *) "led1", configMINIMAL_STACK_SIZE, (void *) 1000, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(LedFlash2, (signed char *) "led2", configMINIMAL_STACK_SIZE, (void *) 1500, tskIDLE_PRIORITY + 3, NULL);

	xQueue = xQueueCreate( 10, sizeof( unsigned char ) );
	xTaskCreate(ButtonControl, (signed char *) "button", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

	vTaskStartScheduler();

	/* just sit and flash all the LEDs quickly if we fail */
	critical_error_blink();
	return 0;
}
