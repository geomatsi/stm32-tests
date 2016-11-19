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

#include <inttypes.h>
#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

/* */

static ssize_t _iord(void *_cookie, char *_buf, size_t _n);
static ssize_t _iowr(void *_cookie, const char *_buf, size_t _n);

static ssize_t _iord(void *_cookie, char *_buf, size_t _n)
{
	/* dont support reading now */
	(void)_cookie;
	(void)_buf;
	(void)_n;
	return 0;
}

static ssize_t _iowr(void *_cookie, const char *_buf, size_t _n)
{
	uint32_t dev = (uint32_t)_cookie;

	int written = 0;
	while (_n-- > 0) {
		usart_send_blocking(dev, *_buf++);
		written++;
	};
	return written;
}

static FILE *usart_setup(uint32_t dev)
{
	/* setup USART1 parameters */
	usart_set_baudrate(dev, 115200);
	usart_set_databits(dev, 8);
	usart_set_parity(dev, USART_PARITY_NONE);
	usart_set_stopbits(dev, USART_CR2_STOP_1_0BIT);
	usart_set_mode(dev, USART_MODE_TX);
	usart_set_flow_control(dev, USART_FLOWCONTROL_NONE);

	/* enable USART */
	usart_enable(dev);

	cookie_io_functions_t stub = { _iord, _iowr, NULL, NULL };
	FILE *fp = fopencookie((void *)dev, "rw+", stub);
	/* Do not buffer the serial line */
	setvbuf(fp, NULL, _IONBF, 0);
	return fp;
}

/* */

static void clock_setup(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();

	/* enable GPIOA clock for LED and UART */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* enable clocks for USART1 */
	rcc_periph_clock_enable(RCC_USART1);
}

static void gpio_setup(void)
{
	/* LED pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

	/* USART1 pins */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO9 | GPIO10);
}

int main(void)
{
	int i, c = 0;
	FILE *fp;

	clock_setup();
	gpio_setup();
	fp = usart_setup(USART1);

	while (1) {
		gpio_toggle(GPIOA, GPIO4);

		fprintf(fp, "Pass: %d\n", c);
		c = (c == 200) ? 0 : c + 1;

		for (i = 0; i < 1000000; i++) {
			__asm__("NOP");
		}
	}

	return 0;
}
