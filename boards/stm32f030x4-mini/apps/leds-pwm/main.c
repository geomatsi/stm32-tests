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

#include <inttypes.h>
#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

static void gpio_setup(void)
{
	/* PA4: setup TIM14 CH1 output */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO4);
	gpio_set_af(GPIOA, GPIO_AF4, GPIO4);

	/* PB1: setup output pin to control LED */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);
}

static void nvic_setup(void)
{
	/* TIM3 interrupt */
	nvic_enable_irq(NVIC_TIM3_IRQ);
	nvic_set_priority(NVIC_TIM3_IRQ, 1);

	nvic_enable_irq(NVIC_TIM14_IRQ);
	nvic_set_priority(NVIC_TIM14_IRQ, 1);
}

void tim3_isr(void)
{
	/* LED toggle */
	gpio_toggle(GPIOB, GPIO1);

	/* clear interrrupt flag */
	TIM_SR(TIM3) &= ~TIM_SR_UIF;
}

static uint32_t duty = 0;

void tim14_isr(void)
{
	if (++duty > 100)
		duty = 0;

	/* update PWM duty cycle */
	TIM_CCR1(TIM14) = duty;

	/* clear interrrupt flag */
	TIM_SR(TIM14) &= ~TIM_SR_UIF;
}

int main(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_TIM3);
	rcc_periph_clock_enable(RCC_TIM14);

	gpio_setup();
	nvic_setup();

	/*
	 * TIM3: simple upcouning mode
	 */

	/* set timer start value */
	TIM_CNT(TIM3) = 1;

	/* set timer prescaler: 48MHz/4800 => 10000 counts per second. */
	TIM_PSC(TIM3) = 4800;

	/* end timer value:  this is reached =>  interrupt is generated */
	TIM_ARR(TIM3) = 10000;

	/* update interrupt enable */
	TIM_DIER(TIM3) |= TIM_DIER_UIE;

	/* start counter */
	TIM_CR1(TIM3) |= TIM_CR1_CEN;

	/*
	 * TIM14: PWM mode
	 */

	/* set timer prescaler: 48MHz/4800 => 10000 counts per second. */
	TIM_PSC(TIM14) = 4800;

	/* ARR defines PWM frequency: 100Hz */
	TIM_ARR(TIM14) = 100;

	/* CCR1 defines PWM duty cycle */
	TIM_CCR1(TIM14) = duty;

	/* mode: PWM mode 1, enable preload register */
	TIM_CCMR1(TIM14) |=
		TIM_CCMR1_OC1M_PWM1 |
		TIM_CCMR1_OC1PE;

	/* main output enable */
	TIM_CCER(TIM14) |= TIM_CCER_CC1E;

	/* BDTR: main output setup -- NOT NEEDED FOR TIM14 */
	/* TIM_BDTR(TIM14) |= TIM_BDTR_MOE; */

	/* update interrupt enable */
	TIM_DIER(TIM14) |= TIM_DIER_UIE;

	/* start counter */
	TIM_CR1(TIM14) |= TIM_CR1_CEN;

	/* force update generation */
	TIM_EGR(TIM14) |= TIM_EGR_UG;

	while (1);

	return 0;
}
