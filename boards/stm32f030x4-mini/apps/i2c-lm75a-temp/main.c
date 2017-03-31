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
	rcc_clock_setup_in_hsi_out_48mhz();

	/* enable GPIOA clock for LED and UART */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* enable clocks for USART1 */
	rcc_periph_clock_enable(RCC_USART1);

	/* enable clocks for I2C1 */
	rcc_periph_clock_enable(RCC_I2C1);
}

static void gpio_setup(void)
{
	/* LED pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

	/* USART1 pins */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2 | GPIO3);

	/* I2C1 pins */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
	gpio_set_af(GPIOA, GPIO_AF4, GPIO9 | GPIO10);
}

/*
 *	I2C on STM32F03x step-by-step without libopencm3
 *
 *	- init i2c
 *		-- disable peripheral before configuration
 *			I2C1_CR1 &= ~I2C_CR1_PE;
 *
 *		-- setup I2C1 timings for SCL = 100KHz when I2CCLK = SYSCLK = 48MHz
 *			I2C1_TIMINGR = \
 *				I2C_TIMINGR_PRESC_VAL(0xb) |
 *				I2C_TIMINGR_SCLDEL_VAL(0x4) |
 *				I2C_TIMINGR_SDADEL_VAL(0x2) |
 *				I2C_TIMINGR_SCLH_VAL(0xf) |
 *				I2C_TIMINGR_SCLL_VAL(0x13);
 *
 *		-- enable peripheral
 *			I2C1_CR1 |= I2C_CR1_PE;
 *
 *	- reading temperature
 *
 *		-- prepare write transfer: request temperature read
 *			I2C1_CR2 = I2C_CR2_NBYTES_VAL(1) | I2C_CR2_SADD_VAL(addr << 1);
 *
 *		-- start 1-byte write transfer: slave address + 1 byte of data
 *			I2C1_CR2 |= I2C_CR2_START;
 *
 *		-- wait for ack
 *			while(!(I2C1_ISR & I2C_ISR_TXIS));
 *
 *		-- transfer LM75A pointer reg value: 0x0 (read temperature)
 *			I2C1_TXDR = 0x0;
 *
 *		-- wait for transfer complete
 *			while(!(I2C1_ISR & I2C_ISR_TC));
 *
 *		-- prepare read transfer: read 2 bytes of temperature data
 *			I2C1_CR2 = I2C_CR2_NBYTES_VAL(2) | I2C_CR2_SADD_VAL(addr << 1) | \
 *				I2C_CR2_AUTOEND | I2C_CR2_RD_WRN;
 *
 *		-- start 2-byte read transfer
 *			I2C1_CR2 |= I2C_CR2_START;
 *
 *		-- read 1st byte
 *			while(!(I2C1_ISR & I2C_ISR_RXNE));
 *			data = I2C1_RXDR;
 *
 *		-- read 2nd byte
 *			while(!(I2C1_ISR & I2C_ISR_RXNE));
 *			data = I2C1_RXDR;
 *
 *
 */

static void i2c_100khz_i2cclk48mhz(uint32_t i2c)
{
	i2c_set_prescaler(i2c, 0xb);
	i2c_set_scl_low_period(i2c, 0x13);
	i2c_set_scl_high_period(i2c, 0xF);
	i2c_set_data_hold_time(i2c, 0x2);
	i2c_set_data_setup_time(i2c, 0x4);
}

int main(void)
{
	uint8_t lm75_addr = 0x48;
	uint8_t temp_reg = 0x0;
	uint8_t data[2];
	int t, c = 0;

	rcc_clock_setup_in_hsi_out_48mhz();
	systick_setup_mhz(48);

	rcc_setup();
	gpio_setup();
	usart_setup();

	/* disable peripheral before configuration */
	i2c_peripheral_disable(I2C1);

	/* setup I2C1 timings for SCL = 100KHz when I2CCLK = SYSCLK = 48MHz */
	i2c_100khz_i2cclk48mhz(I2C1);

	/* enable peripheral */
	i2c_peripheral_enable(I2C1);

	while (1) {

		/* read temperature: 2 bytes */
		i2c_transfer7(I2C1, lm75_addr, &temp_reg, 1, data, 2);

		gpio_toggle(GPIOA, GPIO4);

		/* FIXME: add support for negative temperatures */
		t = (data[0] << 3) + (data[1] >> 5);

		printf("LM75a: reading[%d] temp[%d]\r\n", c++, t*125/1000);
		delay_ms(1000);
	}

	return 0;
}
