#include "reg_manager.h"
#include "common.h"
#include "uart.h"
#include <stdlib.h>
#include <stdio.h>

uint8_t reg_array[REG_COUNT];
void (*reg_handlers[REG_COUNT])(uint8_t);
uint8_t frame_buffer[FRAME_BUFFER_SIZE];

void reg_manager_init(void)
{
	uart_log(__FUNCTION__);
	
	uint8_t index;
	for(index = 0; index < REG_COUNT; index++)
	{
		reg_array[index] = 0;
		reg_handlers[index] = NULL;
	}	
}

void reg_manager_connect_handler(uint8_t reg_id, void (*handler)(uint8_t))
{
	uart_log(__FUNCTION__);
	if((reg_id < REG_COUNT) && (handler != NULL))
	{
		#ifdef DEBUG
		char buf[32];
		sprintf(buf,"reg handler connected, id=%d",reg_id);
		uart_log_v(buf);
		#endif
		reg_handlers[reg_id] = handler;
	}
	else
	{
		uart_log("reg handler not connected");
	}
}

void reg_write(uint8_t *frame)
{
	uart_log(__FUNCTION__);
	
	if((frame[FRAME_REG_ADDR] < REG_COUNT) 	&&
	   (frame[FRAME_REG_COUNT] > 0)		&&		
	   (frame[FRAME_REG_ADDR] + frame[FRAME_REG_COUNT] < REG_COUNT))
	   {
		uart_log("frame valid");
		uint8_t index;
		uint8_t reg_array_base;
		reg_array_base = frame[FRAME_REG_ADDR];
		for(index = 0; index < frame[FRAME_REG_COUNT]; index++)
		{
			#ifdef DEBUG
			char buf[32];
			sprintf(buf,"reg write, addr=%d, value=%d",(reg_array_base + index),frame[FRAME_REG_VALUES + index]);
			uart_log_v(buf);
			#endif
			reg_array[reg_array_base + index] = frame[FRAME_REG_VALUES + index];
			if(reg_handlers[reg_array_base + index] != NULL)
			{
				uart_log("calling reg update handler");
				reg_handlers[reg_array_base + index](reg_array[reg_array_base + index]);
;
			}
		}
	   }
	else
	{
		uart_log("frame invalid");
	}
}

void reg_read(uint8_t *frame)
{
	if((frame[FRAME_REG_ADDR] < REG_COUNT) 	&&
	   (frame[FRAME_REG_COUNT] > 0)		&&		
	   (frame[FRAME_REG_ADDR] + frame[FRAME_REG_COUNT] < REG_COUNT))
	   {
		   uart_log("frame valid");
		uint8_t index;
		uint8_t reg_array_base;
		reg_array_base = frame[FRAME_REG_ADDR];
		for(index = 0; index < frame[FRAME_REG_COUNT]; index++)
		{
			#ifdef DEBUG
			char buf[32];
			sprintf(buf,"reg read, addr=%d, value=%d",(reg_array_base + index),reg_array[reg_array_base + index]);
			uart_log_v(buf);
			#endif
			frame[FRAME_REG_VALUES + index] = reg_array[reg_array_base + index];
		}	
		frame[FRAME_LENGTH] = index + 4;
	   }
	else
	{
		uart_log("frame invalid");
		frame[FRAME_LENGTH] = BASE_FRAME_LENGTH + 1;

		frame[FRAME_COMMAND] = COMMAND_REG_READ;
		frame[FRAME_REG_ADDR] = 0;
		frame[FRAME_REG_COUNT] = 0;
		if(frame[FRAME_REG_ADDR] < REG_COUNT)
		{
			frame[FRAME_REG_VALUES] = E_ADDR_INV;
		}
		else
		{
			frame[FRAME_REG_VALUES] = E_CNT_INV;
		}

	}
}

uint8_t reg_manager_get_reg(uint8_t reg_id)
{
	if(reg_id < REG_COUNT)
	{
		return reg_array[reg_id];
	}
	else
	{
		return 0;
	}
}

void reg_manager_set_reg(uint8_t reg_id, uint8_t reg_value)
{
	if(reg_id < REG_COUNT)
	{
		reg_array[reg_id] = reg_value;
	}
}

void invalid_frame(uint8_t *frame)
{
	/*
	uint8_t len;
	len = sprintf((char *)(frame)," tInvalid frame received\n\r");
	len++;
	frame[0] = len;
	*/
}

void reg_manager_update(void)
{
	if(uart_get_frame(frame_buffer))
	{
		uart_log("reg maganer frame rx complete");
		if(frame_buffer[FRAME_LENGTH] >= BASE_FRAME_LENGTH)
		{
			switch(frame_buffer[FRAME_COMMAND])
			{
				case COMMAND_REG_WRITE:
					uart_log("reg write command");
					reg_write(frame_buffer);
				break;

				case COMMAND_REG_READ:
					uart_log("reg read command");
					reg_read(frame_buffer);
					uart_send_frame(frame_buffer);
				break;

				default:
					uart_log("invalid command");
				break;
			}
		}
		else
		{
			uart_log("frame length invalid");
		}
	}
}


