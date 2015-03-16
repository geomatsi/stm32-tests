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
	systick_set_reload(120);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();

	/* this done last */
	systick_interrupt_enable();
}

/* setup stm32f4 clock to 120MHz
 * TODO: pass clock freq as a parameter
 */

void setup_clocks(void)
{
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);
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
