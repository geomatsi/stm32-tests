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

/* setup stm32f103 clock to 72MHz */

void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

/* Set up a timer to create 1uS ticks. */

void systick_setup(void)
{
	/* clock rate / 1e6 to get 1us interrupt rate */
	systick_set_reload(72);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();

	/* this done last */
	systick_interrupt_enable();
}


