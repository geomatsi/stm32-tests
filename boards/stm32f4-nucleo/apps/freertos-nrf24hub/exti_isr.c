#include <stdio.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <nRF24L01.h>
#include <RF24.h>

/* */

extern QueueHandle_t xButtonQueue;
extern bool exti_falling;

/* */

extern QueueHandle_t xRxQueue;
extern struct rf24 *pnrf;

/* */

void exti15_10_isr(void)
{
	unsigned char msg;
	uint8_t status;

	/* handle user button interrupt */
	if (exti_get_flag_status(EXTI13)) {

		exti_reset_request(EXTI13);

		if (exti_falling) {
			msg = 'P';
			xQueueSendToBackFromISR(xButtonQueue, &msg, NULL);
			exti_falling = false;
			exti_set_trigger(EXTI13, EXTI_TRIGGER_RISING);
		} else {
			msg = 'R';
			xQueueSendToBackFromISR(xButtonQueue, &msg, NULL);
			exti_falling = true;
			exti_set_trigger(EXTI13, EXTI_TRIGGER_FALLING);
		}
	}

	/* handle nrf24 radio interrupt */
	if (exti_get_flag_status(EXTI10)) {

		exti_reset_request(EXTI10);

		status = rf24_get_status(pnrf);

		/* XXX
		 *   It looks like no need to clear interrupts here:
		 *   libnrf24 takes care about their cleanup
		 *   during rx/tx procedures.
		 */

		if (status & (1 << RX_DR)) {
			printf("radio interrupt: RX_DR\n\r");
			msg = 'R';
			xQueueSendToBackFromISR(xRxQueue, &msg, NULL);
		}

		if (status & (1 << TX_DS)) {
			printf("radio interrupt: TX_DS\n\r");
		}

		if (status & (1 << MAX_RT)) {
			printf("radio interrupt: MAX_RT\n\r");
		}
	}

}
