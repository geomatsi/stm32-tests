#include "radio.h"

/* */

uint8_t addr0[] = {'E', 'F', 'C', 'L', 'I'};
uint8_t addr1[] = {'E', 'F', 'S', 'N', '1'};

uint8_t buf[64];

QueueHandle_t xRxQueue;
struct rf24 *pnrf;

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

/* HACK: implement delay API using vTaskDelay
 *   - usable only from tasks
 *   - libnrf24 needs up to ~10us delays => high RATE_HZ is needed
 *
 * TODO: delay API needs to be implemented properly (TIM timers ?)
 */

void delay_ms(int ms)
{
	TickType_t delay = ms * configTICK_RATE_HZ / 1000;
	vTaskDelay(delay);
}

void delay_us(int us)
{
	TickType_t delay = us * configTICK_RATE_HZ / 1000000;
	vTaskDelay(delay);
}

/* */

static void radio_hw_init(void)
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

	rf24_init(pnrf);

	/* */

	rf24_enable_dynamic_payloads(pnrf);

	rf24_open_reading_pipe(pnrf, 0x0 /* pipe number */, addr0);
	rf24_open_reading_pipe(pnrf, 0x1 /* pipe number */, addr1);

	rf24_start_listening(pnrf);
	rf24_print_status(rf24_get_status(pnrf));
}

/* */

bool sensor_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
	sensor_data sensor = {};

	if (!pb_decode(stream, sensor_data_fields, &sensor)) {
		return false;
	}

	printf("sensor[%u] = %u\n", sensor.type, sensor.data);

	return true;
}

void decode_message(uint8_t *buf, uint32_t len)
{
	pb_istream_t stream = pb_istream_from_buffer(buf, len);
	sensor_data_list message = {};
    bool pb_status;

	message.sensor.funcs.decode = &sensor_callback;
	message.sensor.arg = NULL;

	pb_status = pb_decode(&stream, sensor_data_list_fields, &message);

	if (!pb_status) {
		printf("protobuf decoding failed: %s\n", PB_GET_ERROR(&stream));
	}

	return;
}

/* */

void radio_init(void)
{
	xRxQueue = xQueueCreate( 256, sizeof( unsigned char ) );

	if (!xRxQueue) {
		printf("ERROR: can't create radio queue\n\r");
	}

	pnrf = &nrf;
}

/* */

void radio_task(void *Parameters)
{
	(void) Parameters;
	uint32_t more_data;
	unsigned char c;
	uint8_t pipe;
	uint8_t len;

	radio_hw_init();

	printf("started radio task...\n\r");

	while(1) {

		xQueueReceive(xRxQueue, &c, portMAX_DELAY);
		printf("radio_task got %c message\n\r", c);

		while (rf24_available(pnrf, &pipe)) {

			if ((pipe < 0) || (pipe > 5)) {
				printf("WARN: invalid pipe number 0x%02x\n", (int) pipe);
			} else {
				printf("INFO: data ready in pipe 0x%02x\n", pipe);
				memset(buf, 0x0, sizeof(buf));

				len = rf24_get_dynamic_payload_size(pnrf);
				more_data = rf24_read(pnrf, buf, len);
				decode_message(buf, len);

				if (!more_data)
					printf("WARN: RX_FIFO not empty: %d\n", more_data);
			}
		}
	}
}
