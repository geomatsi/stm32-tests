#include "radio.h"

/* */

uint8_t addr0[] = {'E', 'F', 'C', 'L', 'I'};
uint8_t addr1[] = {'E', 'F', 'S', 'N', '1'};

uint8_t buf[64];

/* */

static void f_csn(int level)
{
	(level > 0) ? gpio_set(GPIOA, GPIO9) : gpio_clear(GPIOA, GPIO9);
}

static void f_ce(int level)
{
	(level > 0) ? gpio_set(GPIOA, GPIO8) : gpio_clear(GPIOA, GPIO8);
}

static void f_spi_set_speed(int khz)
{
	/* not implemented */
}

static uint8_t f_spi_xfer(uint8_t dat)
{
	return ((uint8_t) spi_xfer(SPI1, (uint16_t) dat));
}

/* */

struct rf24 nrf = {
	.csn = f_csn,
	.ce = f_ce,
	.spi_set_speed = f_spi_set_speed,
	.spi_xfer = f_spi_xfer,
};

/* UGLY HACK: delay API needs to be properly implemented using e.g. TIM timers */

void delay_ms(int delay)
{
	vTaskDelay(delay);
}

void delay_us(int delay)
{
	vTaskDelay(1);
}

/* */

void radio_init(void)
{
	/* Default configuration from libnrf24:
	 *  - SETUP_RETR:ARD = 100b		~ auto retransmit delay 1500us
	 *  - SETUP_RETR:ARC = 1111b	~ up to 15 retransmits
	 *  - RF_SETUP:RF_PWR = 11b		~ RF output power 0dBm
	 *  - RF_SETUP:RF_DR_* = 0		~ data rate 1Mbps
	 *  - CONFIG:EN_CRC = 1			~ CRC enabled
	 *  - CONFIG:CRCO = 1			~ 16bit CRC mode
	 *  - DYNPD = 0					~ dynamic payload lengths zeroed
	 *  - RF_CH = 76				~ RF channel 76
	 *
	 *  - payload size 32 bytes
	 *  - no ack payload
	 */

	struct rf24 *pnrf = &nrf;

	rf24_init(pnrf);
	rf24_print_status(rf24_get_status(pnrf));

	/* */

	rf24_enable_dynamic_payloads(pnrf);

	rf24_open_reading_pipe(pnrf, 0x0 /* pipe number */, addr0);
	rf24_open_reading_pipe(pnrf, 0x1 /* pipe number */, addr1);

	rf24_start_listening(pnrf);
	rf24_print_status(rf24_get_status(pnrf));
    rf24_print_details(pnrf);
}

/* */

void radio_task(void *Parameters)
{
	(void) Parameters;

	uint32_t more_data;
	uint8_t pipe;
	uint8_t len;

	struct rf24 *pnrf = &nrf;
	TickType_t LastWake;

	radio_init();

	LastWake = xTaskGetTickCount();

	while(1) {

		if (rf24_available(pnrf, &pipe)) {

			if ((pipe < 0) || (pipe > 5)) {
				printf("WARN: invalid pipe number 0x%02x\n", (int) pipe);
			} else {
				printf("INFO: data ready in pipe 0x%02x\n", pipe);
				memset(buf, 0x0, sizeof(buf));

				len = rf24_get_dynamic_payload_size(pnrf);
				more_data = rf24_read(pnrf, buf, len);
				printf("INFO: data [%08x]\n", *buf);

				if (!more_data)
					printf("WARN: RX_FIFO not empty: %d\n", more_data);
			}
		}

		vTaskDelayUntil(&LastWake, 10);
	}
}
