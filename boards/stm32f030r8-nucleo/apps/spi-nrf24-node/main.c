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

#include "pb_encode.h"
#include "pb_decode.h"
#include "msg.pb.h"

#include "clock.h"
#include "delay.h"

/* FIXME: create tinylib and its header for this stuff */
#define size_t  unsigned int
extern int sprintf(char *out, const char *format, ...);
extern int printf(const char *format, ...);
extern void * memset(void *s, int c, size_t n);

/* */

#define PB_LIST_LEN 2

static uint32_t count = 0;

bool sensor_encode_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
	uint32_t data[PB_LIST_LEN];
	sensor_data sensor = {};
	uint32_t idx;

	data[0] = (uint32_t)0xdeadbeef;
	data[1] = (uint32_t)count;

	/* encode  sensor_data */

	for (idx = 0; idx < PB_LIST_LEN; idx++) {
		sensor.type = idx;
		sensor.data = data[idx];

		if (!pb_encode_tag_for_field(stream, field)) {
			printf("protobuf tag encoding failed: %s\n", PB_GET_ERROR(stream));
			return false;
		}

		if (!pb_encode_submessage(stream, sensor_data_fields, &sensor)) {
			printf("protobuf submessage encoding failed: %s\n", PB_GET_ERROR(stream));
			return false;
		}
	};

	return true;
}

/* */

int putchar(int c)
{
	uint8_t ch = (uint8_t)c;
	usart_send_blocking(USART2, ch);
	return 0;
}

static void usart_setup(void)
{
	/* setup USART2 parameters */
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_stopbits(USART2, USART_CR2_STOP_1_0BIT);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	/* enable USART2 */
	usart_enable(USART2);
}

static void rcc_setup(void)
{
	/* enable GPIO clock for LED/UART/SPI */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);

	/* enable clocks for USART2 */
	rcc_periph_clock_enable(RCC_USART2);

	/* enable clocks for SPI1 */
	rcc_periph_clock_enable(RCC_SPI1);
}

static void pinmux_setup(void)
{
	/* disable RFM69HW on Wireless Shield v1.0 which is on the same spi */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
	gpio_set(GPIOC, GPIO7);

	/* LED pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);

	/* USART2 pins */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2 | GPIO3);

	/* NRF24: SPI1 pins - MOSI/MISO/SCK */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6 | GPIO7);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH,  GPIO5 | GPIO6 | GPIO7);
	gpio_set_af(GPIOA, GPIO_AF0, GPIO5 | GPIO6 | GPIO7);

	/* NRF24: CE pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH,  GPIO8);

	/* NRF24: CS pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO9);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH,  GPIO9);

	/* NRF24: IRQ pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO10);

	/* start with spi communication disabled */
	gpio_set(GPIOA, GPIO9);
	gpio_clear(GPIOA, GPIO8);
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
	(level > 0) ? gpio_set(GPIOA, GPIO9) : gpio_clear(GPIOA, GPIO9);
}

void f_ce(int level)
{
	(level > 0) ? gpio_set(GPIOA, GPIO8) : gpio_clear(GPIOA, GPIO8);
}

uint8_t f_spi_xfer(uint8_t dat)
{
	spi_send8(SPI1, dat);
	return spi_read8(SPI1);
}

struct rf24 nrf24_ops = {
	.csn = f_csn,
	.ce = f_ce,
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
	uint8_t addr[] = {0xE1, 0xE1, 0xE1, 0xE1, 0xE1};
	uint32_t node_id = 2001;

	struct rf24 *nrf = &nrf24_ops;
	uint8_t buf[32];

	node_sensor_list message = node_sensor_list_init_default;
	pb_ostream_t stream;
	bool pb_status;
	size_t pb_len;

	enum rf24_tx_status ret;

	rcc_clock_setup_in_hsi_out_48mhz();
	systick_setup_mhz(48);

	rcc_setup();
	pinmux_setup();
	usart_setup();
	spi_setup();

	printf("radio init...\r\n");
	rf24_init(&nrf24_ops);

	printf("radio setup...\r\n");
	rf24_enable_dyn_payload(nrf);
	rf24_set_retries(nrf, 0xf /* retry delay 4000us */, 5 /* retries */);
	rf24_set_channel(nrf, 10);
	rf24_set_data_rate(nrf, RF24_RATE_250K);
	rf24_set_crc_mode(nrf, RF24_CRC_16_BITS);
	rf24_set_pa_level(nrf, RF24_PA_MIN);

	rf24_setup_ptx(nrf, addr);
	rf24_start_ptx(nrf);

	printf("start xmit cycle...\r\n");
	while (1) {

		printf("xmit pkt #%u\n", (unsigned int)++count);
		memset(buf, 0x0, sizeof(buf));
		stream = pb_ostream_from_buffer(buf, sizeof(buf));

		/* static message part */
		message.node.node = node_id;

		/* repeated message part */
		message.sensor.funcs.encode = &sensor_encode_callback;

		pb_status = pb_encode(&stream, node_sensor_list_fields, &message);
		pb_len = stream.bytes_written;

		if (!pb_status) {
			printf("nanopb encoding failed: %s\n", PB_GET_ERROR(&stream));
		} else {
			printf("nanopb encoded %u bytes\n", pb_len);
		}

		ret = rf24_send(nrf, buf, pb_len);
		if (ret != RF24_TX_OK) {
			printf("send error: %d\n", ret);
			rf24_flush_tx(nrf);
			rf24_flush_rx(nrf);
		} else {
			printf("written %d bytes\n", pb_len);
		}

		gpio_toggle(GPIOA, GPIO5);
		delay_ms(5000);
	}

	return 0;
}
