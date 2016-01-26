#include "frame_manager.h"
#include "fifo.h"
#include "uart.h"
#include "common.h"
#include <string.h>

static struct fifo rx_fifo;
static struct fifo tx_fifo;

void frame_manager_DBG(void)
{
    fifo_put(&tx_fifo,'D');
	fifo_put(&tx_fifo,'a');
	fifo_put(&tx_fifo,'r');
	fifo_put(&tx_fifo,'o');
	fifo_put(&tx_fifo,'\n');
	fifo_put(&tx_fifo,'\r');
	uart_send_tx_fifo();
}

void frame_manager_init(void)
{
	fifo_init(&rx_fifo);
	fifo_init(&tx_fifo);
	uart_init(&rx_fifo,&tx_fifo);

}

uint8_t frame_manager_get_frame(struct frame *f)
{
    static uint8_t header_found = 0;
    uint8_t ret_val;
    ret_val = 0;

    while(!header_found && (fifo_get_data_count(&rx_fifo) >= FRAME_HEADER_LENGTH))
    {
        //printf("Header Search, fifo data_count=%d, START=%c, END=%c\n\r",rx_fifo.data_count,fifo_get_data_at(&rx_fifo,FRAME_START),fifo_get_data_at(&rx_fifo,FRAME_HEADER_END));
        if((fifo_get_data_at(&rx_fifo,FRAME_START) == HEADER_START_CODE) &&
           (fifo_get_data_at(&rx_fifo,FRAME_HEADER_END) == HEADER_END_CODE))
           {
             //printf("Header found\n\r");
             header_found = 1;
             break;
           }
           else
           {
            //printf("Header NOT found\n\r");
            //header_found = 0;
            fifo_flush_data(&rx_fifo,1);
           }
    }

    if(header_found)
    {
        uint8_t tmp_length;
        tmp_length = fifo_get_data_at(&rx_fifo,FRAME_LENGTH);
        //printf("FRAME LENGTH=%d\n\r",tmp_length);
        if(fifo_get_data_count(&rx_fifo) >= tmp_length)
        {
            if(fifo_get_data_at(&rx_fifo,tmp_length-1) == FRAME_END_CODE)
            {
                //printf("FRAME FOUND\n\r");

                uint8_t data,index;
                fifo_get(&rx_fifo,&data);//get $

                fifo_get(&rx_fifo,&data);//get len
                f->length = data;

                fifo_get(&rx_fifo,&data);//get cmd
                f->command = data;

                fifo_get(&rx_fifo,&data);//get reg_addr
                f->reg_addr = data;

                fifo_get(&rx_fifo,&data);//get reg_count
                f->reg_count = data;

                fifo_get(&rx_fifo,&data);//get #

                for(index = 0; index < (f->length - (FRAME_HEADER_LENGTH + 1)); index++)
                {
                    fifo_get(&rx_fifo,&data);
                    f->reg_values[index] = data;
                }
                fifo_get(&rx_fifo,&data);//get @
                ret_val = 1;
            }
            else
            {
                //printf("FRAME invalid, flush header\n\r");
                fifo_flush_data(&rx_fifo,FRAME_HEADER_LENGTH);
            }
            header_found = 0;
        }
    }


	return ret_val;
}

uint8_t frame_manager_send_frame(struct frame *f)
{
    uint8_t index;
	fifo_put(&tx_fifo,'$');
	fifo_put(&tx_fifo,f->length);
	fifo_put(&tx_fifo,f->command);
	fifo_put(&tx_fifo,f->reg_addr);
	fifo_put(&tx_fifo,f->reg_count);
	fifo_put(&tx_fifo,'#');
    for(index = 0; index < (f->length - (FRAME_HEADER_LENGTH + 1)); index++)
    {
        fifo_put(&tx_fifo,f->reg_values[index]);
    }
    fifo_put(&tx_fifo,'@');
    uart_send_tx_fifo();
    return 1;
}

