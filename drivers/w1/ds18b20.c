#include <inttypes.h>

#include "ds18b20.h"
#include "delay.h"

static res_t resolution = R12BIT;

int ds18b20_crc_check(const uint8_t *data, uint32_t len)
{
	uint8_t crc, byte;
	uint8_t fst, inb;
	int i;

	crc = 0x0;

	while (len--) {
		byte = *data++;

		for (i = 0; i < 8; i++) {
			inb = (byte >> i) & 0x1;
			fst = crc & 0x1;
			crc = (fst ^ inb) ? ((crc >> 1) ^ 0x8c) : (crc >> 1);
		}
	}

	return (crc == 0x0);
}

int ds18b20_set_res(res_t res)
{
	int ret = 1;

	/* TODO: currently support only default 12bit resolution */

	switch (res) {
		case R9BIT:
			ret = 0;
			break;
		case R10BIT:
			ret = 0;
			break;
		case R11BIT:
			ret = 0;
			break;
		case R12BIT:
			resolution = res;
			break;
		default:
			ret = 0;
			break;
	}

	return ret;
}

/* calculate temperature according to resolution */
int ds18b20_get_temp(uint8_t msb, uint8_t lsb)
{
	uint16_t val;
	int temp;

	/* TODO: currently support only default 12bit resolution */

	switch (resolution) {
		case R9BIT:
		case R10BIT:
		case R11BIT:
			temp = -100;
			break;
		case R12BIT:
			val = (msb << 8) | lsb;
			temp = (val & 0x07FF) >> 4;
			temp = (val & 0xF800) ? -temp : temp;
			break;
		default:
			temp = -200;
			break;
	}

	return temp;
}
