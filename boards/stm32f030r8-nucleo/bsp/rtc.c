/*
 * This file is part of the libopencm3 project.
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
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/pwr.h>

#include "rtc-utils.h"

void rtc_read_calendar(struct rtc_time *time, struct rtc_date *date)
{
	uint32_t reg;

	rtc_unlock();

	/* copy current time to shadow registers */
	RTC_ISR &= ~(RTC_ISR_RSF);
	while (!(RTC_ISR & RTC_ISR_RSF));

	reg = RTC_TR;
	if (time) {
		if (reg & RTC_TR_PM)
			time->pm = 1;
		time->ht = (reg >> RTC_TR_HT_SHIFT) & RTC_TR_HT_MASK;
		time->hu = (reg >> RTC_TR_HU_SHIFT) & RTC_TR_HU_MASK;
		time->mnt = (reg >> RTC_TR_MNT_SHIFT) & RTC_TR_MNT_MASK;
		time->mnu = (reg >> RTC_TR_MNU_SHIFT) & RTC_TR_MNU_MASK;
		time->st = (reg >> RTC_TR_ST_SHIFT) & RTC_TR_ST_MASK;
		time->su = (reg >> RTC_TR_SU_SHIFT) & RTC_TR_SU_MASK;
	}

	reg = RTC_DR;
	if (date) {
		date->yt = (reg >> RTC_DR_YT_SHIFT) & RTC_DR_YT_MASK;
		date->yu = (reg >> RTC_DR_YU_SHIFT) & RTC_DR_YU_MASK;
		date->wdu = (reg >> RTC_DR_WDU_SHIFT) & RTC_DR_WDU_MASK;
		date->mt = (reg & RTC_DR_MT) >> RTC_DR_MT_SHIFT;
		date->mu = (reg >> RTC_DR_MU_SHIFT) & RTC_DR_MU_MASK;
		date->dt = (reg >> RTC_DR_DT_SHIFT) & RTC_DR_DT_MASK;
		date->du = (reg >> RTC_DR_DU_SHIFT) & RTC_DR_DU_MASK;
	}

	rtc_lock();
}

void rtc_set_alarm(struct rtc_alarm *alarm)
{
	uint32_t reg = 0;

	if (!alarm)
		return;

	rtc_unlock();

	/* disable alarm A to modify it */
	RTC_CR &= ~RTC_CR_ALRAE;

	/* wait until it is allowed to modify alarm A settings */
	while (!(RTC_ISR & RTC_ISR_ALRAWF));

	if (alarm->msk4) {
		reg |= RTC_ALRMXR_MSK4;
	} else {
		if (alarm->wdsel)
			reg |= RTC_ALRMXR_WDSEL;
		reg |= (alarm->dt & RTC_ALRMXR_DT_MASK) << RTC_ALRMXR_DT_SHIFT;
		reg |= (alarm->du & RTC_ALRMXR_DU_MASK) << RTC_ALRMXR_DU_SHIFT;
	}

	if (alarm->msk3) {
		reg |= RTC_ALRMXR_MSK3;
	} else {
		if (alarm->pm)
			reg |= RTC_ALRMXR_PM;
		reg |= (alarm->ht & RTC_ALRMXR_HT_MASK) << RTC_ALRMXR_HT_SHIFT;
		reg |= (alarm->hu & RTC_ALRMXR_HU_MASK) << RTC_ALRMXR_HU_SHIFT;
	}

	if (alarm->msk2) {
		reg |= RTC_ALRMXR_MSK2;
	} else {
		reg |= (alarm->mnt & RTC_ALRMXR_MNT_MASK) << RTC_ALRMXR_MNT_SHIFT;
		reg |= (alarm->mnu & RTC_ALRMXR_MNU_MASK) << RTC_ALRMXR_MNU_SHIFT;
	}

	if (alarm->msk1) {
		reg |= RTC_ALRMXR_MSK1;
	} else {
		reg |= (alarm->st & RTC_ALRMXR_ST_MASK) << RTC_ALRMXR_ST_SHIFT;
		reg |= (alarm->su & RTC_ALRMXR_SU_MASK) << RTC_ALRMXR_SU_SHIFT;
	}

	/* write next alarm date and time */
	RTC_ALRMAR = reg;

	/* enable alarm A event and interrupt */
	RTC_CR |= RTC_CR_ALRAIE | RTC_CR_ALRAE;

	rtc_lock();

}
