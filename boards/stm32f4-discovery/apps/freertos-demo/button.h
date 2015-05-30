#ifndef BUTTON_H
#define BUTTON_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

void button_task(void *Parameters);
void button_init(void);

#endif /* BUTTON_H */
