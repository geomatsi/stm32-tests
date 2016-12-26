/*
 * Based on stm32f0-discovery usart_stdio example from libopencm3-example project
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2012 Karl Palsson <karlp@tweak.net.au>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>

#include "clock.h"
#include "delay.h"

extern int printf(const char *format, ...);

int putchar(int c)
{
	uint8_t ch = (uint8_t)c;
	usart_send_blocking(USART1, ch);
	return 0;
}

static void usart_setup(void)
{
	/* setup USART1 parameters */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_CR2_STOP_1_0BIT);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* enable USART1 */
	usart_enable(USART1);
}

static void rcc_setup(void)
{
	/* enable GPIOA clock for LED and UART */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* enable clocks for USART1 */
	rcc_periph_clock_enable(RCC_USART1);

	/* enable clocks for ADC */
	/* TODO */
}

static void gpio_setup(void)
{
	/* LED pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

	/* USART1 pins */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2 | GPIO3);

	/* ADC pins */
	/* TODO */
}

int main(void)
{
	int i = 0;

	rcc_clock_setup_in_hsi_out_48mhz();
	systick_setup_mhz(48);

	rcc_setup();
	gpio_setup();
	usart_setup();

	while (1) {

		gpio_toggle(GPIOA, GPIO4);
		printf("XXXX[%d]\n", i++);
		delay_ms(500);
	}

	return 0;
}
