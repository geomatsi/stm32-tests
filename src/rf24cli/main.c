/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include <RF24.h>

#include "rf24cli.h"
#include "clock.h"

/* */

static struct rf24 *nrf;

/* */

static void usart_setup(void)
{
	/* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_GPIOA);

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
}

int putchar(int c)
{
	uint16_t ch = (uint16_t) c;
	usart_send_blocking(USART2, ch);
	return 0;
}

/* */

void leds_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
}

/* */

int main(void)
{
	uint32_t i = 0;
	uint8_t val;

	/* */

	setup_clocks();

	usart_setup();
	leds_setup();

	nrf = radio_init();

	/* */

	while (1) {

		gpio_toggle(GPIOA, GPIO5);

		delay_ms(1000);

		printf("cycle: 0x%08x\r\n", ++i);

		val = rf24_get_status(nrf);
		printf("rf24_status: 0x%02x\r\n", val);

		val = (uint8_t) rf24_get_data_rate(nrf);
		printf("data rate = 0x%02x\r\n", val);

		val = rf24_read_register(nrf, 0x5);
		printf("channel = 0x%02x\r\n", val);

		val = (uint8_t) rf24_get_crc_length(nrf);
		printf("crc length = 0x%02x\r\n", val);

		val = (uint8_t) rf24_is_p_variant(nrf);
		printf("model = 0x%02x\r\n\r\n", val);
	}
}
