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

#include "rf24srv.h"
#include "clock.h"

/* */

static struct rf24 *nrf;

/* UART debug port: RX = PA3 TX = PA2 */

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
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);

	gpio_set(GPIOD, GPIO12);
	gpio_clear(GPIOD, GPIO14);
}

/* */

int main(void)
{
	uint8_t addr[] = {'E', 'F', 'C', 'L', 'I'};
	uint8_t buf[20];

	uint32_t more_data;
	uint8_t pipe;

	/* */

	setup_clocks();

	usart_setup();
	leds_setup();

	nrf = radio_init();

	/* */

	rf24_set_payload_size(nrf, sizeof(buf));
	rf24_open_reading_pipe(nrf, 0x0 /* pipe number */, addr);
	rf24_start_listening(nrf);

	/* */

	while (1) {

		if (rf24_available(nrf, &pipe)) {
			printf("INFO: data ready in pipe 0x%02x\n", pipe);
			memset(buf, 0x0, sizeof(buf));
			more_data = rf24_read(nrf, buf, sizeof(buf));
			printf("INFO: received data [%s]\n", buf);
			if (!more_data)
				printf("WARN: RX_FIFO not empty: %d\n", more_data);
			gpio_toggle(GPIOD, GPIO12);
		}

		gpio_toggle(GPIOD, GPIO14);
		delay_ms(10);
	}
}
