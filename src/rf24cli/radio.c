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
#include <libopencm3/stm32/spi.h>

#include <RF24.h>

#include "rf24cli.h"

/*
 *	Connectivity between nRF24L01 and stm32f4-discovery:
 *		MOSI <-> 
 *		MISO <-> 
 *		SCK  <-> 
 *		CSN  <-> 
 *		CE   <-> 
 *		IRQ  <-> 
 *		VCC  <-> 3V
 *		GND  <-> GND
 */

/* */

static void radio_spi_setup(void)
{
	/* for spi */
	rcc_periph_clock_enable(RCC_SPI1);

	/* for nRF24L01 spi pins */
	rcc_periph_clock_enable(RCC_GPIOA);

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

/* */

static void radio_pins_setup(void)
{
	/* for nRF24L01 gpio pins */
	rcc_periph_clock_enable(RCC_GPIOE);

	/* disable MEMS (LIS302DL) which is on the same spi */
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	gpio_set(GPIOE, GPIO3);

	/* setup GPIOE pins for nRF24L01 CE, CSN, IRQ */
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
	gpio_mode_setup(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO6);

	/* start with spi communication disabled */
	gpio_set(GPIOE, GPIO4);
	gpio_clear(GPIOE, GPIO5);
}

/* */

void f_csn(int level)
{
	(level > 0) ? gpio_set(GPIOE, GPIO4) : gpio_clear(GPIOE, GPIO4);
}

void f_ce(int level)
{
	(level > 0) ? gpio_set(GPIOE, GPIO5) : gpio_clear(GPIOE, GPIO5);
}

void f_spi_set_speed(int khz)
{
	/* not implemented */
}

uint8_t f_spi_xfer(uint8_t dat)
{
	return ((uint8_t) spi_xfer(SPI1, (uint16_t) dat));
}

struct rf24 nrf = {
	.csn = f_csn,
	.ce = f_ce,
	.spi_set_speed = f_spi_set_speed,
	.spi_xfer = f_spi_xfer,
};

/* */

struct rf24* radio_init(void)
{
	radio_pins_setup();
	radio_spi_setup();

	rf24_init(&nrf);

	return &nrf;
}
