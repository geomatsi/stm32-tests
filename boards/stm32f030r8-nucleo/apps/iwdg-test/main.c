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

#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/gpio.h>

void blink(uint32_t num, uint32_t delay)
{
	volatile uint32_t i,j;

	for (i = 0; i < num; i++)
	{
		for (j = 0; j < delay; j++)
		{
			__asm__("nop");
		}

		gpio_toggle(GPIOA, GPIO5);
	}
}

int main(void)
{

	rcc_clock_setup_in_hsi_out_48mhz();

	/* PA5: setup output pin to control LED */
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);

	/* start-up flickering to indicate reset */
	blink(10, 100000);

	/* init watchdog */
	iwdg_set_period_ms(3000);
	iwdg_start();

	/* slow blinking until watchdog fires after ~5sec */
	while (1) {
		blink(~0U, 1000000);
	}

	return 0;
}
