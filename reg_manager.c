#include "reg_manager.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>

uint8_t reg_array[REG_COUNT];
void (*reg_handlers[REG_COUNT])(void);


void reg_manager_init(void)
{
	uint8_t index;
	for(index = 0; index < REG_COUNT; index++)
	{
		reg_array[index] = 0;
		reg_handlers[index] = NULL;
	}	
}

void reg_manager_connect_handler(uint8_t reg_id, void (*handler)(void))
{
	if((reg_id < REG_COUNT) && (handler != NULL))
	{
		reg_handlers[reg_id] = handler;
	}
}

uint8_t reg_write(uint8_t *frame)
{
	if((frame[FRAME_REG_ADDR] < REG_COUNT) 	&&
	   (frame[FRAME_REG_COUNT] > 0)		&&		
	   (frame[FRAME_REG_ADDR] + frame[FRAME_REG_COUNT] < REG_COUNT))
	   {
		uint8_t index;
		uint8_t reg_array_base;
		reg_array_base = frame[FRAME_REG_ADDR];
		for(index = 0; index < frame[FRAME_REG_COUNT]; index++)
		{
			reg_array[reg_array_base + index] = frame[FRAME_REG_VALUES + index];
			if(reg_handlers[reg_array_base + index] != NULL)
			{
				reg_handlers[reg_array_base + index]();
			}
		}
	   }

	return 0;
}

uint8_t reg_read(uint8_t *frame)
{
	return 0;
}

void invalid_frame(uint8_t *frame)
{
	uint8_t len;
	len = sprintf((char *)(frame)," tInvalid frame received\n\r");
	len++;
	frame[0] = len;
}




uint8_t reg_manager_parse(uint8_t *frame)
{
	uint8_t ret_val;
	ret_val = 0;
	if((frame != NULL) && (frame[FRAME_LENGTH] > 0))
	{
		switch(frame[FRAME_COMMAND])
		{
			case 'w':
				if(0 == reg_write(frame))
				{
					invalid_frame(frame);
					ret_val = 1;
				}
			break;

			case 'r':
				if(0 == reg_read(frame))
				{
					invalid_frame(frame);
				}
				ret_val = 1;
			break;

			default:
				invalid_frame(frame);
				ret_val = 1;
			break;
		}
		
	}
	return ret_val;
}


