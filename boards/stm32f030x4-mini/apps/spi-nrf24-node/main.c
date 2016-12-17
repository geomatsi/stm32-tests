/*
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
#include <libopencm3/stm32/spi.h>

#include <RF24.h>

#include "clock.h"
#include "delay.h"

/* FIXME: create tinylib and its header for this stuff */
#define size_t  unsigned int
extern int sprintf(char *out, const char *format, ...);
extern int printf(const char *format, ...);
extern void * memset(void *s, int c, size_t n);

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

	/* enable clocks for SPI1 */
	rcc_periph_clock_enable(RCC_SPI1);
}

static void pinmux_setup(void)
{
	/* LED pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

	/* USART1 pins */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2 | GPIO3);

	/* NRF24: SPI1 pins - MOSI/MISO/SCK */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6 | GPIO7);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH,  GPIO5 | GPIO6 | GPIO7);
	gpio_set_af(GPIOA, GPIO_AF0, GPIO5 | GPIO6 | GPIO7);

	/* NRF24: CE pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH,  GPIO0);

	/* NRF24: CS pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO1);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH,  GPIO0);
}

static void spi_setup(void)
{
	spi_set_master_mode(SPI1);
	spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_16);
	spi_set_clock_polarity_0(SPI1);
	spi_set_clock_phase_0(SPI1);
	spi_set_full_duplex_mode(SPI1);
	spi_set_unidirectional_mode(SPI1);
	spi_set_data_size(SPI1, SPI_CR2_DS_8BIT);
	spi_enable_software_slave_management(SPI1);
	spi_send_msb_first(SPI1);
	spi_set_nss_high(SPI1);
	spi_disable_crc(SPI1);
	spi_fifo_reception_threshold_8bit(SPI1);
	spi_i2s_mode_spi_mode(SPI1);
	spi_enable(SPI1);
}

/* */

void f_csn(int level)
{
	(level > 0) ? gpio_set(GPIOA, GPIO1) : gpio_clear(GPIOA, GPIO1);
}

void f_ce(int level)
{
	(level > 0) ? gpio_set(GPIOA, GPIO0) : gpio_clear(GPIOA, GPIO0);
}

void f_spi_set_speed(int khz)
{
	/* not implemented */
}

uint8_t f_spi_xfer(uint8_t dat)
{
	spi_send8(SPI1, dat);
	return spi_read8(SPI1);
}

struct rf24 nrf24_ops = {
	.csn = f_csn,
	.ce = f_ce,
	.spi_set_speed = f_spi_set_speed,
	.spi_xfer = f_spi_xfer,
};

void delay_us(int delay)
{
	/*
	 * This function is needed by libnrf24: 10us, 130us, 1500us delays.
	 * However libnrf24 is not very demanding to those timings.
	 * That is why this workaround: wait a little longer...
	 */

	delay_ms(1);
}

/* */

int main(void)
{
	uint8_t addr[] = {'E', 'F', 'C', 'L', 'I'};
	struct rf24 *nrf = &nrf24_ops;
	uint8_t buf[20];

	int count = 0;
	int ret;

	rcc_clock_setup_in_hsi_out_48mhz();
	systick_setup_mhz(48);

	rcc_setup();
	pinmux_setup();
	usart_setup();
	spi_setup();

	printf("radio init...\r\n");
	rf24_init(&nrf24_ops);

	printf("radio setup...\r\n");
	rf24_stop_listening(nrf);
	rf24_set_payload_size(nrf, sizeof(buf));
	rf24_set_retries(nrf, 0xf /* retry delay 4000us */, 5 /* retries */);
	rf24_open_writing_pipe(nrf, addr);
	rf24_power_up(nrf);

	printf("start xmit cycle...\r\n");
	while (1) {

		memset(buf, 0x0, sizeof(buf));
		sprintf((char *) buf, "xmit 0x%04x", count++);
		printf("xmit buffer: sizeof(%s) = %d\r\n", buf, sizeof(buf));

		ret = rf24_write(nrf, buf, sizeof(buf));
		if (ret) {
			printf("write error: %d\n", ret);
			rf24_flush_tx(nrf);
			rf24_flush_rx(nrf);
		}

		gpio_toggle(GPIOA, GPIO4);
		delay_ms(1000);
	}

	return 0;
}
