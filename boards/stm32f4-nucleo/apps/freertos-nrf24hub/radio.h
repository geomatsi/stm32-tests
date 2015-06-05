#ifndef RADIO_H
#define RADIO_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include <RF24.h>
#include <nRF24L01.h>
#include <rf24log.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* */

void radio_task(void *Parameters);
void radio_init(void);

#endif /* RADIO_H */
