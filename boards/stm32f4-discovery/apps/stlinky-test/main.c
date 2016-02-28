/*
 * Based on stm32f4-discovery miniblink example from libopencm3 project
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2015 matsi
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

#include <stlinky.h>

#include "clock.h"
#include "delay.h"

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);

	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);
}

int main(void)
{
	char rx[20] = {0};
	char tx[] = "hello";

	/* NOTE: execution blocks here until st-term is connected */
	printf("start %s ...\n", tx);

	clock_setup();
	systick_setup();

	gpio_setup();
	gpio_set(GPIOD, GPIO12);
	gpio_set(GPIOD, GPIO13);
	gpio_set(GPIOD, GPIO14);
	gpio_set(GPIOD, GPIO15);

	while (1) {

		gpio_toggle(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);

		delay_ms(500);

		if (stlinky_avail(&g_stlinky_term)) {
			stlinky_rx(&g_stlinky_term, rx, sizeof(rx));
			tx[0] = rx[0];

			printf("stlinky: %d <-> %s\n", (int) rx[0], tx);
		}
	}

	return 0;
}
