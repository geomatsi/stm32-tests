#ifndef MISC_H
#define MISC_H

#include <libopencm3/stm32/gpio.h>

void error_blink(uint32_t port, uint16_t pins);
void test_blink(uint32_t port, uint16_t pins, uint32_t num);
void assert_blink(void);

#endif /* MISC_H */
