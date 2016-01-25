#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

//#include "reg_manager.h"
#include "uart.h"
#include "common.h"
//#include "event_manager.h"
//#include "I2Cdev.h"
//#include "MPU6050.h"
//#include "pid_manager.h"
//#include "motor_manager.h"
#include "fifo.h"
//char dupa[32] = "TMP_LOG_TEST\n\r";

/*
uint8_t frame_buffer[32];

uint8_t main_tick;
uint8_t pid_x_id, pid_y_id, pid_z_id;

void dbg_led_blink(void)
{
	DBG_LED3_TOGGLE;
}

void dbg_led0_blink(uint8_t value)
{
	if(value)DBG_LED0_ON;
	else DBG_LED0_OFF;
}

void dbg_led1_blink(uint8_t value)
{
	if(value)DBG_LED1_ON;
	else DBG_LED1_OFF;
}

void dbg_led2_blink(uint8_t value)
{
	if(value)DBG_LED2_ON;
	else DBG_LED2_OFF;
}

void dbg_led3_blink(uint8_t value)
{
	if(value)DBG_LED3_ON;
	else DBG_LED3_OFF;
}

void main_loop_tick(void)
{
	main_tick = 1;
}

#define COMMAND_UPDATE_PID 1

void command_handler(uint8_t value)
{
	uart_log(__func__);
	switch(value)
	{
		case COMMAND_UPDATE_PID:
			uart_log("COMMAND_UPDATE_PID");
			pid_manager_reinit_pid(pid_x_id);
			pid_manager_reinit_pid(pid_y_id);
			pid_manager_reinit_pid(pid_z_id);
		break;

	}
}	


int16_t map(int16_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
	return (int16_t)(((int32_t)x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}


void calculate_setpoints(int16_t *thrust, int16_t *pitch, int16_t *roll, int16_t *yaw)
{
	int16_t temp_i16;
	int8_t temp_i8;

	temp_i16 = (int16_t)(reg_manager_get_reg(REG_THRUST));
     	*thrust = map(temp_i16,0,255,THRUST_MIN_VALUE,THRUST_MAX_VALUE);

        temp_i8 = (int8_t)(reg_manager_get_reg(REG_PITCH));
        temp_i16 = (int16_t)temp_i8;
        *pitch = map(temp_i16,-127,127,PITCH_MIN_VALUE,PITCH_MAX_VALUE);

	temp_i8 = (int8_t)(reg_manager_get_reg(REG_ROLL));
	temp_i16 = (int16_t)temp_i8;
	*roll = map(temp_i16,-127,127,ROLL_MIN_VALUE,ROLL_MAX_VALUE);

	temp_i8 =(int8_t)(reg_manager_get_reg(REG_YAW));
	temp_i16 = (int16_t)temp_i8;
	*yaw = map(temp_i16,-127,127,YAW_MIN_VALUE,YAW_MAX_VALUE);
}

*/

int main(void)
{	

	struct fifo FIFO_RX;
	struct fifo FIFO_TX;

	
	DBG_LED0_INIT;
	DBG_LED1_INIT;
	DBG_LED2_INIT;
	DBG_LED3_INIT;
	DBG_LED0_OFF;
	DBG_LED1_OFF;
	DBG_LED2_OFF;
	DBG_LED3_OFF;

	fifo_init(&FIFO_RX);
	fifo_init(&FIFO_TX);
	uart_init(&FIFO_RX, &FIFO_TX);

	
	sei();

	/*fifo_init(&FIFO1);
	uint8_t index;
	index = 1;
	while(fifo_put(&FIFO1,index))
	{
		index++;
		//index = (index ^ (index << 3));
	}
	while(fifo_get(&FIFO1,&index))
	{
		char buf[32];
		sprintf(buf,"Value=%d",index);
		uart_log_v(buf);
	}*/

	uint8_t tmp;
	while(1)
	{
		if(fifo_get(&FIFO_RX,&tmp))
		{
			if(tmp == 'w')
			{
				fifo_put(&FIFO_TX,'D');
				fifo_put(&FIFO_TX,'a');
				fifo_put(&FIFO_TX,'r');
				fifo_put(&FIFO_TX,'i');
				fifo_put(&FIFO_TX,'u');
				fifo_put(&FIFO_TX,'s');
				fifo_put(&FIFO_TX,'z');
				fifo_put(&FIFO_TX,' ');
				fifo_put(&FIFO_TX,'w');
				fifo_put(&FIFO_TX,'y');
				fifo_put(&FIFO_TX,'m');
				fifo_put(&FIFO_TX,'i');
				fifo_put(&FIFO_TX,'a');
				fifo_put(&FIFO_TX,'t');
				fifo_put(&FIFO_TX,'a');
				fifo_put(&FIFO_TX,'\n');
				fifo_put(&FIFO_TX,'\r');
				
				uart_send_tx_fifo();
			}
		}
				
	}
	//uint8_t index;
	//index = 250;
	
	/*while(fifo_put(&FIFO1,index))
	{
		index++;
		//index = (index ^ (index << 3));
	}
	while(fifo_get(&FIFO1,&index))
	{
		char buf[32];
		sprintf(buf,"Value=%d",index);
		uart_log_v(buf);
	}

	while(1)
	{

	}*/
	/*
	int16_t ax, ay, az, gx, gy, gz;
	int16_t pid_x_out, pid_y_out, pid_z_out;
	int16_t thrust,pitch,roll,yaw;
	main_tick = 0;
	ax = 0;
	ay = 0;
	az = 0;
	gx = 0;
	gy = 0;
	gz = 0;
	
	DBG_LED0_INIT;
	DBG_LED1_INIT;
	DBG_LED2_INIT;
	DBG_LED3_INIT;
	DBG_LED0_OFF;
	DBG_LED1_OFF;
	DBG_LED2_OFF;
	DBG_LED3_OFF;
	
	uart_init();
	event_manager_init();
	reg_manager_init();
	motor_manager_init();

	reg_manager_connect_handler(REG_COMMAND,command_handler);

	pid_manager_init();	
	pid_x_id = pid_manager_create_pid(REG_PID_X_PH,REG_PID_X_IH,REG_PID_X_DH);
	pid_y_id = pid_manager_create_pid(REG_PID_Y_PH,REG_PID_Y_IH,REG_PID_Y_DH);
	pid_z_id = pid_manager_create_pid(REG_PID_Z_PH,REG_PID_Z_IH,REG_PID_Z_DH);

	uart_connect_event();
	I2C_dev_init();
	event_manager_connect_event(1000,dbg_led_blink,EVENT_CONTINOUS);
	event_manager_connect_event(10,main_loop_tick,EVENT_CONTINOUS);
	

	sei();

	MPU6050_initialize();
	
	while(1)
	{
		event_manager_update();
		reg_manager_update();

		if(main_tick)
		{
			main_tick = 0;
			MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
		
			calculate_setpoints(&thrust, &pitch, &roll, &yaw);		

			pid_x_out = pid_manager_update_pid(pid_x_id,pitch,gx);
			pid_y_out = pid_manager_update_pid(pid_y_id,roll,gy);	
			pid_z_out = pid_manager_update_pid(pid_z_id,yaw,gz);	

			motor_manager_update(thrust, pid_x_out, pid_y_out,pid_z_out);

		}
	}
	*/
	return 0;
}
