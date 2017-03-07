#ifndef __DS18B20_H__
#define __DS18B20_H__

/* Dallas/Maxim DS18B20 */

/* commands */
#define SEARCH_ROM	0xF0
#define READ_ROM	0x33
#define MATCH_ROM	0x55
#define SKIP_ROM	0xCC
#define CONVERT_T	0x44
#define WRITE_PAD	0x4E
#define READ_PAD	0xBE
#define COPY_PAD	0x48

/* resolution */
typedef enum { R9BIT, R10BIT, R11BIT, R12BIT } res_t;

/* check crc */
int ds18b20_crc_check(const uint8_t *, uint32_t);

/* set resolution: 9-12 bits */
int ds18b20_set_res(res_t);

/* calculate temperature according to resolution */
int ds18b20_get_temp(uint8_t msb, uint8_t lsb);

#endif /* __DS18B20_H__ */
