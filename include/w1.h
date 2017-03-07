#ifndef __W1_H__
#define __W1_H__

/* 1-wire */
int w1_init_transaction(void);
void w1_send_byte(uint8_t);
uint8_t w1_recv_byte(void);

#endif /* __W1_H__ */
