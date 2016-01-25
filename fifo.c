#include "fifo.h"
#include "uart.h"
#include <stdio.h>

void fifo_init(struct fifo *f)
{
	//uart_log(__func__);
	f->w_index = 0;
	f->r_index = 0;
	f->data_count = 0;	
}

uint8_t fifo_put(struct fifo *f, uint8_t data)
{
	if(f->data_count < FIFO_SIZE)
	{
		f->buff[f->w_index] = data;
		f->w_index = ((f->w_index + 1) % FIFO_SIZE);
		f->data_count++;
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t fifo_get(struct fifo *f, uint8_t *data)
{
	if(f->data_count > 0)
	{
		*data = f->buff[f->r_index];
		f->r_index = ((f->r_index + 1) % FIFO_SIZE);
		f->data_count--;
		return 1;
	}
	else
	{
		return 0;
	}
}


