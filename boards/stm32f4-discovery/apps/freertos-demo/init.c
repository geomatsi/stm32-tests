#include "init.h"

/* */

/* FreeRTOS needs this variable */
uint32_t SystemCoreClock;

/* */

void hw_init(void)
{
	/* TODO: sync SystemCoreClock with rcc_clock_setup_xxx settings */
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);
	SystemCoreClock = 120000000;

	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_SYSCFG);

	/* leds */

	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);

	gpio_clear(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	/* uart2 port: RX = PA3 TX = PA2 */

	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2| GPIO3);

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART2);

	/* user button */

	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);

	/* ATTENTION
	 * It is essential that interrupt handlers that make use of the FreeRTOS API
	 * have a logical priority equal to or below that set by
	 * the configMAX_SYSCALL_INTERRUPT_PRIORITY
	 *
	 * NOTE
	 * In ARM Cortex-M cores, numerically low priority values are used
	 * to specify logically high interrupt priorities
	 *
	 * NOTE
	 * Here we use minimal logical priority for user button interrupt
	 */

	nvic_set_priority(NVIC_EXTI0_IRQ, configKERNEL_INTERRUPT_PRIORITY);

	/* enable EXTI0 interrupt */

	nvic_enable_irq(NVIC_EXTI0_IRQ);

	/* configure the EXTI subsystem */

	exti_select_source(EXTI0, GPIOA);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);
	exti_enable_request(EXTI0);

	/*
		http://www.freertos.org/RTOS-Cortex-M3-M4.html

		Preempt priority and subpriority
		If you are using an STM32 then ensure that all the priority bits are assigned to
		be preempt priority bits by the following function before FreeRTOS is started:
		  - NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4) if you are using CMSIS
		  - scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP16_NOSUB) if you are using libopencm3
	*/

	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP16_NOSUB);

	/* DON'T PUT ANYTHING HERE AFTER NVIC PRIO GROUP ARE PROPERLY CONFIGURED FOR FREERTOS */
}


