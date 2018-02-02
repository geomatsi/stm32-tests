#ifndef _UART_H_
#define _UART_H_

#include <libopencm3/stm32/usart.h>

#include "FreeRTOS.h"
#include "task.h"

/* */

extern int printf(const char *format, ...);
int putchar(int c);

void uart_task(void *Parameters);

#endif /* _UART_H_ */
