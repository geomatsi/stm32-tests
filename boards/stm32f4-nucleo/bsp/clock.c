/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Damjan Marion <damjan.marion@gmail.com>
 * Copyright (C) 2011 Mark Panajotovic <marko@electrontube.org>
 * Copyright (C) 2013 Chuck McManis <cmcmanis@mcmanis.com>
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
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>

/* monotonically increasing number of microseconds from reset
 * NB: overflows every ~5800 centuries
 * TODO: need to check if 64bit tick counter is ok
 */

volatile uint64_t system_micros;

/* define non-empty sys_tick_handler
 *   - see libopencm3/lib/cm3/vector.c
 */

/* Called when systick fires */
void sys_tick_handler(void)
{
	system_micros++;
}

/* Set up a timer to create 1uS ticks. */

static void systick_setup(void)
{
	/* clock rate / 1e6 to get 1us interrupt rate */
	systick_set_reload(84);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();

	/* this done last */
	systick_interrupt_enable();
}

/* setup stm32f401re-nucleo clock to 84MHz
 *
 * TODO
 *   - unify clock setup for nucleo boards with libopencm3
 *   - submit patch to libopencm3
 */

static void rcc_clock_setup_in_hsi_out_84mhz(void)
{
	/* Enable power control block. */
	rcc_periph_clock_enable(RCC_PWR);

	/* Disable voltage scaling. */
	pwr_set_vos_scale(SCALE2);

	/* Enable internal high-speed oscillator. */
	rcc_osc_on(HSI);
	rcc_wait_for_osc_ready(HSI);

	/* Select HSI as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_HSI);

	/* Set prescalers for AHB, ADC, ABP1, ABP2. */
	rcc_set_hpre(RCC_CFGR_HPRE_DIV_NONE);
	rcc_set_ppre1(RCC_CFGR_PPRE_DIV_2);
	rcc_set_ppre2(RCC_CFGR_PPRE_DIV_NONE);

	/* */
	rcc_set_main_pll_hsi(16, 336, 4, 7);

	/* Enable PLL oscillator and wait for it to stabilize. */
	rcc_osc_on(PLL);
	rcc_wait_for_osc_ready(PLL);

	/* Configure flash settings. */
	flash_set_ws(FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_2WS);

	/* Select PLL as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_PLL);

	/* Wait for PLL clock to be selected. */
	rcc_wait_for_sysclk_status(PLL);

	/* Set the peripheral clock frequencies used */
	rcc_ahb_frequency  = 84000000;
	rcc_apb1_frequency = 42000000;
	rcc_apb2_frequency = 84000000;
}

void setup_clocks(void)
{
	rcc_clock_setup_in_hsi_out_84mhz();
	systick_setup();
}

/* delay API */

void delay_us(int delay)
{
	uint64_t wake = system_micros + (uint64_t) delay;
	while (wake > system_micros);
}

void delay_ms(int delay)
{
	uint64_t wake = system_micros + (uint64_t) (delay*1000);
	while (wake > system_micros);
}
