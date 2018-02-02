#ifndef _INIT_H_
#define _INIT_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/usart.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>

#include "FreeRTOSConfig.h"

void hw_init(void);

#endif /* _INIT_H_ */
