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

static void gpio_setup(void)
{
	/* LED pin */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
}

/* set next alarm: current time + min:sec */
void rtc_set_alarm_time(int min, int sec)
{
	uint32_t reg;
	int mu, mt;
	int su, st;

	rtc_unlock();

	/* step 0: read current min/sec values in BCD format */
	RTC_ISR &= ~(RTC_ISR_RSF);
	while (!(RTC_ISR & RTC_ISR_RSF));

	reg = RTC_TR;

	mt = (reg >> RTC_TR_MNT_SHIFT) & RTC_TR_MNT_MASK;
	mu = (reg >> RTC_TR_MNU_SHIFT) & RTC_TR_MNU_MASK;

	st = (reg >> RTC_TR_ST_SHIFT) & RTC_TR_ST_MASK;
	su = (reg >> RTC_TR_SU_SHIFT) & RTC_TR_SU_MASK;

	/* need to read date as well even if no need to use it */
	reg = RTC_DR;

	/* step 1: disable alarm A to modify it */
	RTC_CR &= ~RTC_CR_ALRAE;

	/* step 2: wait until it is allowed to modify alarm A settings */
	while (!(RTC_ISR & RTC_ISR_ALRAWF));

	/* step 3: program next alarm event */

	/* bcd min/sec math: calculate next alarm time */
	su += sec;

	st += su/10;
	su %= 10;

	mu += min + st/6;
	st %= 6;

	mt += mu / 10;
	mu %= 10;

	/* hu += mt/6 : for this test app we don't care about hours */
	mt %= 6;

	/* for this test app we don't care about date/hours match */
	reg = RTC_ALRMXR_MSK4 | RTC_ALRMXR_MSK3;

	reg |= (mt & RTC_TR_MNT_MASK) << RTC_TR_MNT_SHIFT;
	reg |= (mu & RTC_TR_MNU_MASK) << RTC_TR_MNU_SHIFT;
	reg |= (st & RTC_TR_ST_MASK) << RTC_TR_ST_SHIFT;
	reg |= (su & RTC_TR_SU_MASK) << RTC_TR_SU_SHIFT;

	RTC_ALRMAR = reg;

	/* step4: enable alarm A and its interrupt */
	RTC_CR |= RTC_CR_ALRAIE | RTC_CR_ALRAE;

	rtc_lock();
}

void rtc_isr(void)
{
	rtc_clear_alarm_flag();
	exti_reset_request(EXTI17);
	gpio_toggle(GPIOA, GPIO4);
	rtc_set_alarm_time(0, 15);
}

void rtc_setup(void)
{
	/* enable LSI clock */
	rcc_osc_on(RCC_LSI);
	rcc_wait_for_osc_ready(RCC_LSI);

	/* select LSI clock for RTC */
	rcc_set_rtc_clock_source(RCC_LSI);
	rcc_enable_rtc_clock();
}

int main(void)
{
	rcc_clock_setup_in_hsi_out_24mhz();

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

	rtc_set_alarm_time(0, 5);

	while (1);

	return 0;
}
