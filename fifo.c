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

uint8_t fifo_get_data_count(struct fifo *f)
{
    //printf("%s()\n\r",__FUNCTION__);
    return f->data_count;
}

uint8_t fifo_get_data_at(struct fifo *f, uint8_t index)
{
    //printf("%s()\n\r",__FUNCTION__);
    if(index < f->data_count)
    {

        uint8_t index_fifo;
        index_fifo = ((f->r_index + index) % FIFO_SIZE);
        //printf("index=%d, data=%d\n\r",index,f->buff[index_fifo]);
        return f->buff[index_fifo];
    }
    else
    {
        return 0;
    }
}

void fifo_flush_data(struct fifo *f, uint8_t data_count)
{
    //printf("%s()\n\r",__FUNCTION__);
    if(data_count <= f->data_count)
    {
        f->data_count = f->data_count - data_count;
        f->r_index = ((f->r_index + data_count) % FIFO_SIZE);
        //printf("data_count=%d\n\r",f->data_count);
    }
    else
    {
        f->data_count = 0;
        f->r_index = f->w_index;
        //printf("data_count=%d\n\r",f->data_count);
    }
}


