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
#include <libopencm3/stm32/spi.h>

#include <stlinky.h>

#include "clock.h"
#include "delay.h"

static void spi_setup(void)
{
	/* for spi */
	rcc_periph_clock_enable(RCC_SPI1);

	/* for spi signal pins */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* for LIS302DL spi chip select */
	rcc_periph_clock_enable(RCC_GPIOE);

	/* setup GPIOE3 pin for LIS302DL spi chip select */
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);

	/* start with spi communication disabled */
	gpio_set(GPIOE, GPIO3);

	/* setup GPIO pins for AF5 for SPI1 signals */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6 | GPIO7);
	gpio_set_af(GPIOA, GPIO_AF5, GPIO5 | GPIO6 | GPIO7);

	/* spi initialization */
	spi_set_master_mode(SPI1);
	spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_4);
	spi_set_clock_polarity_0(SPI1);
	spi_set_clock_phase_0(SPI1);
	spi_set_full_duplex_mode(SPI1);
	spi_set_unidirectional_mode(SPI1);
	spi_enable_software_slave_management(SPI1);
	spi_send_msb_first(SPI1);
	spi_set_dff_8bit(SPI1);
	spi_set_nss_high(SPI1);
	SPI_I2SCFGR(SPI1) &= ~SPI_I2SCFGR_I2SMOD;
	spi_enable(SPI1);
}

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);

	/* init LED pins */
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);

	/* turn on all the LEDs */
	gpio_set(GPIOD, GPIO12);
	gpio_set(GPIOD, GPIO13);
	gpio_set(GPIOD, GPIO14);
	gpio_set(GPIOD, GPIO15);
}

int main(void)
{
	char rx[20] = {0};
	char tx[] = "hello world";

	/* reg = WHO_AM_I | READ_CMD */
	uint16_t reg = 0x80 | 0xF;
	uint16_t dummy = 0x0;
	uint16_t val;

	/* NOTE: execution blocks here until st-term is connected */
	printf("start %s ...\n", tx);

	clock_setup();
	systick_setup();

	gpio_setup();
	spi_setup();

	/* */

	while (1) {

		gpio_toggle(GPIOD, GPIO12);
		gpio_toggle(GPIOD, GPIO13);
		gpio_toggle(GPIOD, GPIO14);
		gpio_toggle(GPIOD, GPIO15);

		delay_ms(1000);

		if (stlinky_avail(&g_stlinky_term)) {
			stlinky_rx(&g_stlinky_term, rx, sizeof(rx));
			tx[0] = rx[0];

			switch (rx[0]) {
				case 32: /* spi read on SPACE */
					gpio_clear(GPIOE, GPIO3);
					val = spi_xfer(SPI1, reg);
					val = spi_xfer(SPI1, dummy);
					gpio_set(GPIOE, GPIO3);
					printf("spi read 0x%04x\n", val);
					break;
				default:
					printf("key pressed: code('%c') = %d\n", (char) rx[0], (int) rx[0]);
					break;
			}
		}
	}

	return 0;
}
