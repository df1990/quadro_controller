#include "pid_manager.h"
#include "reg_manager.h"
#include "pid.h"
#include "uart.h"
#include <stdio.h>

struct pid_item {
	uint8_t pid_created;
	uint8_t p_addr;
	uint8_t i_addr;
	uint8_t d_addr;
	struct PID_DATA pid;
};

struct pid_item pid_array[PID_COUNT];



void pid_manager_init(void)
{
	//uart_log(__func__);
	uint8_t index;
	for(index = 0; index < PID_COUNT; index++)
	{
		pid_array[index].pid_created = 0;
		pid_array[index].p_addr = 0;
		pid_array[index].i_addr = 0;
		pid_array[index].d_addr = 0;
		pid_Init(0,0,0,&(pid_array[index].pid));
	}

}

uint8_t pid_manager_create_pid(uint8_t pid_p_addr, uint8_t pid_i_addr, uint8_t pid_d_addr)
{
	//uart_log(__func__);
	uint8_t index;
	for(index = 0; index < PID_COUNT; index++)
	{
		if(pid_array[index].pid_created == 0)
		{
			pid_array[index].pid_created = 1;
			pid_array[index].p_addr = pid_p_addr;
			pid_array[index].i_addr = pid_i_addr;
			pid_array[index].d_addr = pid_d_addr;

			return (index + 1);
		}
	}
	//uart_log("cannot create pid");
	return 0;
}

uint8_t pid_manager_reinit_pid(uint8_t pid_id)
{
	//uart_log(__func__);
	if((pid_id > 0) && (pid_id <= PID_COUNT))
	{
		uint8_t index = pid_id - 1;

		uint16_t temp_p;
         	uint16_t temp_i;
         	uint16_t temp_d;

		temp_p = 0;
       		temp_p = (uint16_t)(reg_manager_get_reg(pid_array[index].p_addr));
       		temp_p <<= 8;
	        temp_p |= (uint16_t)(reg_manager_get_reg(pid_array[index].p_addr + 1));
         	temp_p &= 0x7FFF;

         	temp_i = 0;
         	temp_i = (uint16_t)(reg_manager_get_reg(pid_array[index].i_addr));
         	temp_i <<= 8;
         	temp_i |= (uint16_t)(reg_manager_get_reg(pid_array[index].i_addr + 1));
         	temp_i &= 0x7FFF;

		temp_d = 0;
         	temp_d = (uint16_t)(reg_manager_get_reg(pid_array[index].d_addr));
         	temp_d <<= 8;
         	temp_d |= (uint16_t)(reg_manager_get_reg(pid_array[index].d_addr + 1));
         	temp_d &= 0x7FFF;

	        pid_Reinit((int16_t)(temp_p), (int16_t)(temp_i), (int16_t)(temp_d), &(pid_array[index].pid));


	}
 	return 0;
}

int16_t pid_manager_update_pid(uint8_t pid_id, int16_t set_val, int16_t meas_val)
{
	uint16_t ret_val;
	ret_val = 0;

	if((pid_id > 0) && (pid_id <= PID_COUNT))
	{
		uint8_t index = pid_id - 1;

		ret_val = pid_Controller(set_val, meas_val,&(pid_array[index].pid));
	}
	return ret_val;

}


