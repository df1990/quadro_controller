#ifndef _FIFO_H_
#define _FIFO_H_

#include <avr/io.h>

#define FIFO_SIZE 32

struct fifo {
	uint8_t w_index;
	uint8_t r_index;
	uint8_t data_count;
	uint8_t buff[FIFO_SIZE];
};

void fifo_init(struct fifo *f);
uint8_t fifo_put(struct fifo *f, uint8_t data);
uint8_t fifo_get(struct fifo *f, uint8_t *data);

#endif
