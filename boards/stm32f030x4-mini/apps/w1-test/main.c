/*
 * Based on stm32f1 time interrupt example from libopencm3 project
 *
 * Copyright (C) 2010 Thomas Otto <tommi@viadmin.org>
 * Copyright (C) 2016 Sergey Matyukevich <geomatsi@gmail.com>
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

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>

#include "delay.h"
#include "w1.h"
#include "ds18b20.h"

extern int printf(const char *format, ...);

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

int putchar(int c)
{
	uint8_t ch = (uint8_t)c;
	usart_send_blocking(USART1, ch);
	return 0;
}

static void gpio_setup(void)
{
	/* PB1: w1 pin */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);

	/* PA4: LED */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

	/* USART1 pin: TX only PA2 */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2);
}

static void nvic_setup(void)
{
	/* TIM3 interrupt */
	nvic_enable_irq(NVIC_TIM3_IRQ);
	nvic_set_priority(NVIC_TIM3_IRQ, 1);
}

static volatile uint32_t ready = 0;

void tim3_isr(void)
{
	ready = 1;

	/* clear interrrupt flag */
	TIM_SR(TIM3) &= ~TIM_SR_UIF;

	/* stop counter */
	TIM_CR1(TIM3) &= ~TIM_CR1_CEN;
}

void tim3_prepare(uint16_t psc, uint16_t arr)
{
	/*
	 * TIM3: simple upcouning mode
	 */

	/* set timer start value */
	TIM_CNT(TIM3) = 1;

	/* set timer prescaler
	 *   - TIM_PSC = 48 : 48_000_000 Hz / 48 = 1_000_000 ticks per second
	 *   - TIM_ARR = N : irq is generated after N usecs
	 */
	TIM_PSC(TIM3) = psc;

	/* end timer value:  this is reached =>  interrupt is generated */
	TIM_ARR(TIM3) = arr;

	/* update interrupt enable */
	TIM_DIER(TIM3) |= TIM_DIER_UIE;

	/* start counter */
	TIM_CR1(TIM3) |= TIM_CR1_CEN;
}

/* FIXME:
 *   - need properly handle 16bit overflows
 *   - current range: 0 <= msec <= 65535
 *   - current range: 0 <= usec <= 65535
 */
void delay_ms(int msec)
{
	ready = 0;
	tim3_prepare(48000, msec);
	while(!ready);
}

void delay_us(int usec)
{
	ready = 0;
	tim3_prepare(48, usec);
	while(!ready);
}

int main(void)
{
	uint8_t data[9];
	uint8_t i;
	int temp;
	int ret;

	rcc_clock_setup_in_hsi_out_48mhz();

	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_TIM3);

	gpio_setup();
	usart_setup();
	nvic_setup();

#if 0
	while (1) {
		gpio_toggle(GPIOA, GPIO4);
		printf("OK\n\r");

		delay_ms(1000);
	}
#else

	/* w1 example */

	if (!ds18b20_set_res(R12BIT)) {
		printf("WARN: couldn't set resolution\n");
	}

	/* main loop */

	while (1) {

		/* reset and check presence */
		ret = w1_init_transaction();
		if (!ret) {
			printf("presence not detected...\n\r");
			delay_ms(1000);
			continue;
		}

#if 1

		/* skip ROM: next command can be broadcasted */
		w1_send_byte(SKIP_ROM);

		/* start single temperature conversion */
		w1_send_byte(CONVERT_T);

		/* temperature conversion takes ~1sec */
		delay_ms(1000);

		/* reset and check presence */
		ret = w1_init_transaction();
		if (!ret) {
			printf("presence after conversion not detected...\n\r");
			delay_ms(2000);
			continue;
		}

		/* skip ROM: careful !!! works only for one device on bus: next command is unicast */
		w1_send_byte(SKIP_ROM);

		/* read scratchpad */
		w1_send_byte(READ_PAD);

		/* get all scratchpad bytes */
		for (i = 0; i < 9; i++) {
			data[i] = w1_recv_byte();
		}

		/* check crc */
		printf("CRC: %s\n\r", ds18b20_crc_check(data, 9) ? "OK" : "FAIL");
		delay_ms(100);

		/* calculate temperature */
		temp = ds18b20_get_temp(data[1], data[0]);

		printf("temperature %c%d\n\r", (temp >= 0) ? '+' : ' ', temp);

#else
		/* short test command sequence: it should read 1w family code 0x28 */
		w1_send_byte(READ_ROM);
		data[0] = w1_recv_byte();
		printf("ROM[0] = %d\n\r", data[0]);

#endif

		/* wait before starting next cycle */
		delay_ms(1000);
	}
#endif
}
