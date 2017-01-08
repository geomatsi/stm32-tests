/*
 * Based on stm32f0-discovery adc example from libopencm3-example project
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
#include <libopencm3/stm32/adc.h>

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
	/* USART1 pin: TX only PA2 */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2);

	/* USART1 settings */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_CR2_STOP_1_0BIT);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	usart_enable(USART1);
}

static void led_setup(void)
{
	/* LED pin: PA4 */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
}


static void adc_setup(void)
{
	/* pin PB1: ADC_IN9, VREF: ADC_IN17, TEMP: ADC_IN16 */
	uint8_t channels[] = { 9, ADC_CHANNEL_VREF /*, ADC_CHANNEL_TEMP */ };

	/* ADC pin: PB1 */
	gpio_mode_setup(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);

	/* ADC settings */
	adc_power_off(ADC1);
	adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
	adc_calibrate(ADC1);
	adc_set_operation_mode(ADC1, ADC_MODE_SEQUENTIAL);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	adc_enable_temperature_sensor();
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_071DOT5);
	adc_set_regular_sequence(ADC1, 2, channels);
	adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
	adc_disable_analog_watchdog(ADC1);
	adc_enable_vrefint();
	adc_power_on(ADC1);

	/* Wait for ADC starting up. */
	delay_ms(100);

}

static void rcc_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_ADC);
}

int main(void)
{
	unsigned int v;
	uint16_t v1;
	uint16_t v2;

	rcc_clock_setup_in_hsi_out_48mhz();
	systick_setup_mhz(48);

	rcc_setup();
	adc_setup();
	led_setup();
	usart_setup();

	while (1) {
		/* conv #1: PB1 ~ ADC CH9 */
		adc_start_conversion_regular(ADC1);
		while (!(adc_eoc(ADC1)));
		v1 = adc_read_regular(ADC1);

		/* conv #2: VREF ~ ADC CH17 */
		adc_start_conversion_regular(ADC1);
		while (!(adc_eoc(ADC1)));
		v2 = adc_read_regular(ADC1);

		/* STM32F030x4/6/8/C RM0360 Reference manual
		 * Chapter 12.9:
		 * The internal voltage reference (V REFINT ) provides a stable
		 * (bandgap) voltage output for the ADC and Comparators. V
		 * REFINT is internally connected to the ADC_IN17 input channel.
		 * The precise voltage of V REFINT is individually measured for
		 * each part by ST during production test and stored in the
		 * system memory area. It is accessible in read-only mode.
		 *
		 * STM32F030x4/6/8/C datasheet - production data
		 * Chapter 6.3.4:
		 * Vref internal voltage reference: typical 1200mV
		 *
		 * CH17 (internal reference voltage 1200mV) is sampled
		 * in order to calculate the voltage sampled on CH9:
		 * Vin:  CH9 (PB1) input voltage
		 * Vref: C17 (Vref) input voltage
		 *
		 * Vref / v2 ~ mV-per-bit, then
		 * Vin = bits * mV-per-bit = v1 * (Vref / v2)
		 *
		 * In order to carefully handle integer division, the following
		 * calculation is performed:
		 * Vin = (v1 * Vref) / v2
		 *
		 * STM32F030x4/6/8/C datasheet - production data
		 * Chapter 3.10.2:
		 * VREFINT_CAL (2 bytes value) is stored in 0x1FFFF7BA-0x1FFFF7BB
		 *
		 * Note: this calibrated ADC output for Vref can be accessed in
		 * libopencm3 as follows: unsigned int vref = ST_VREFINT_CAL;
		 * FIXME: not sure how we can use it, maybe to introduce
		 * temperature corrections comparing ST_VREFINT_CAL and CH17
		 * readings ?
		 *
		 */

		v = 1200 * v1 / v2;

		printf("PB1: %u mV\n", v);
		gpio_toggle(GPIOA, GPIO4);
		delay_ms(1000);
	}

	return 0;
}
