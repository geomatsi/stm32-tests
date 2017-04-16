/*
 * Based on stm32l-discovery button-irq-printf-lowpower from libopencm3-example
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
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/exti.h>

#include <libopencm3/cm3/nvic.h>

#include "rtc-utils.h"

static void gpio_setup(void)
{
	/* LED pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
}

/* set next alarm: current time + min:sec */
void configure_next_alarm(int min, int sec)
{
	struct rtc_alarm alarm = {};
	struct rtc_time time = {};
	int val;

	/* read current min/sec values */
	rtc_read_calendar(&time, 0);

	/* BCD min/sec math: calculate next alarm */
	val = time.su + sec;
	alarm.su = val % 10;

	val = time.st + val / 10;
	alarm.st = val % 6;

	val = time.mnu + min + val / 6;
	alarm.mnu = val % 10;

	val = time.mnt + val / 10;
	alarm.mnt = val % 6;

	/* this app cares about min/sec only */
	alarm.msk1 = 0;
	alarm.msk2 = 0;
	alarm.msk3 = 1;
	alarm.msk4 = 1;

	/* set new alarm */
	rtc_set_alarm(&alarm);
}

void rtc_isr(void)
{
	rtc_clear_alarm_flag();
	exti_reset_request(EXTI17);
	gpio_toggle(GPIOA, GPIO5);
	configure_next_alarm(0, 10);
}

void rtc_setup(void)
{
	/* reset RTC */
	rcc_periph_reset_pulse(RST_BACKUPDOMAIN);

	/* enable LSI clock */
	rcc_osc_on(RCC_LSI);
	rcc_wait_for_osc_ready(RCC_LSI);

	/* select LSI clock for RTC */
	rtc_unlock();
	rcc_set_rtc_clock_source(RCC_LSI);
	rcc_enable_rtc_clock();
	rtc_lock();
}

int main(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_RTC);
	rcc_periph_clock_enable(RCC_PWR);

	/* enable rtc unlocking */
	pwr_disable_backup_domain_write_protect();

	rtc_setup();
	gpio_setup();

	/* enable rtc irq */
	nvic_enable_irq(NVIC_RTC_IRQ);
	nvic_set_priority(NVIC_RTC_IRQ, 1);

	/* EXTI line 17 is connected to the RTC Alarm event */
	exti_set_trigger(EXTI17, EXTI_TRIGGER_RISING);
	exti_enable_request(EXTI17);

	configure_next_alarm(0, 3);

	while (1);

	return 0;
}
