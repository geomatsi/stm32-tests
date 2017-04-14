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
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

/* monotonically increasing number of milliseconds from reset
 * NB: overflows every ~5850000 centuries
 *
 * NOTE: microseconds are not suitable for low frequency MCU
 * since ~6-48 clock cycles between subsequent systick interrupts
 * will be almost completely consumed by systick handler itself
 *
 * TODO: need to check if 64bit tick counter is ok
 */

volatile uint64_t system_ms = 0;

/* define non-empty sys_tick_handler
 *   - see libopencm3/lib/cm3/vector.c
 */

/* called when systick fires */
void sys_tick_handler(void)
{
	system_ms++;
}

/* set up a timer to create 1ms ticks */
void systick_setup_mhz(uint32_t mhz)
{
	/* set systick clock to CPU clock */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);

	/* clock rate mhz*1e6/1e3 to get 1ms systick interrupt rate */
	systick_set_reload(mhz*1000);

	systick_counter_enable();

	/* this done last */
	systick_interrupt_enable();
}

/* delay API */

void delay_ms(int delay)
{
	uint64_t wake = system_ms + (uint64_t)delay;
	while (wake > system_ms);
}

__attribute__((weak)) void delay_us(int delay)
{
	/* FIXME: NOT YET SUPPORTED, USE APP SPECIFIC HACKS FOR NOW */
}
