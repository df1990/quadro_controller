#include "reg_manager.h"
#include "frame_manager.h"
#include "common.h"
//#include "uart.h"
#include <stdlib.h>
#include <stdio.h>

uint8_t reg_array[REG_COUNT];
void (*reg_handlers[REG_COUNT])(uint8_t);

static struct frame quadro_frame;

void reg_manager_init(void)
{
	uint8_t index;
	for(index = 0; index < REG_COUNT; index++)
	{
		reg_array[index] = 0;
		reg_handlers[index] = NULL;
	}

	frame_manager_init();
}

void reg_manager_connect_handler(uint8_t reg_id, void (*handler)(uint8_t))
{
	if((reg_id < REG_COUNT) && (handler != NULL))
	{
		reg_handlers[reg_id] = handler;
	}
}

void invalid_frame(struct frame *f, uint8_t err_code)
{
	f->length = 7;
	f->command = 'e';
	f->reg_addr = err_code;
	f->reg_count = 0;
}

void reg_write(struct frame *f)
{
    if((f->reg_addr < REG_COUNT) &&
        (f->reg_count > 0) &&
        (f->reg_addr + f->reg_count - 1 < REG_COUNT))
        {
            uint8_t index;
            uint8_t reg_array_base;
            reg_array_base = f->reg_addr;
            for(index = 0; index < f->reg_count; index++)
            {
                reg_array[reg_array_base + index] = f->reg_values[index];
                if(reg_handlers[reg_array_base + index] != NULL)
                {
                    reg_handlers[reg_array_base + index](reg_array[reg_array_base + index]);
                }
            }
            f->length = FRAME_NO_PAYLOAD_LENGTH;
        }
        else if(f->reg_addr >= REG_COUNT)
        {
            invalid_frame(f,ERR_ADDR_INV);
        }
        else
        {
            invalid_frame(f,ERR_CNT_INV);
        }
}

void reg_read(struct frame *f)
{
    if((f->reg_addr < REG_COUNT) &&
        (f->reg_count > 0) &&
        (f->reg_addr + f->reg_count - 1 < REG_COUNT))
        {
            uint8_t index;
            uint8_t reg_array_base;
            reg_array_base = f->reg_addr;
            for(index = 0; index < f->reg_count; index++)
            {
                f->reg_values[index] = reg_array[reg_array_base + index];
            }
            f->length = index + FRAME_NO_PAYLOAD_LENGTH;

        }
        else if(f->reg_addr >= REG_COUNT)
        {
            invalid_frame(f,ERR_ADDR_INV);
        }
        else
        {
            invalid_frame(f,ERR_CNT_INV);
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



void reg_manager_update(void)
{
	if(frame_manager_get_frame(&quadro_frame))
	{
        switch(quadro_frame.command)
        {
            case COMMAND_REG_WRITE:
                reg_write(&quadro_frame);
            break;

            case COMMAND_REG_READ:
                reg_read(&quadro_frame);
            break;

            default:
                invalid_frame(&quadro_frame,ERR_CMD_INV);
            break;
        }
        frame_manager_send_frame(&quadro_frame);
	}
}


