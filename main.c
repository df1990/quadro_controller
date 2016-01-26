#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "common.h"


#include "fifo.h"
#include "uart.h"
#include "frame_manager.h"
#include "reg_manager.h"

#include "event_manager.h"

#include "I2Cdev.h"
#include "MPU6050.h"

#include "pid_manager.h"
#include "setpoints_calc.h"

#include "motor_manager.h"

static uint8_t main_tick;
static uint8_t pid_x_id, pid_y_id, pid_z_id;

void dbg_led_blink(void)
{
	DBG_LED3_TOGGLE;
}

void main_loop_tick(void)
{
	main_tick = 1;
}

/*

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
*/



void pid_x_update(uint8_t value)
{
    if(value)
    {
        pid_manager_reinit_pid(pid_x_id);
    }
}

void pid_y_update(uint8_t value)
{
    if(value)
    {
        pid_manager_reinit_pid(pid_y_id);
    }
}

void pid_z_update(uint8_t value)
{
    if(value)
    {
        pid_manager_reinit_pid(pid_z_id);
    }
}



int main(void)
{

    int16_t ax, ay, az, gx, gy, gz;
    int16_t thrust,pitch,roll,yaw;
    int16_t pid_x_out, pid_y_out, pid_z_out;

	DBG_LED0_INIT;
	DBG_LED1_INIT;
	DBG_LED2_INIT;
	DBG_LED3_INIT;
	DBG_LED0_OFF;
	DBG_LED1_OFF;
	DBG_LED2_OFF;
	DBG_LED3_OFF;


    reg_manager_init();

    event_manager_init();
    event_manager_connect_event(1000,dbg_led_blink,EVENT_CONTINOUS);
    event_manager_connect_event(10,main_loop_tick,EVENT_CONTINOUS);

    pid_manager_init();
    pid_x_id = pid_manager_create_pid(REG_PID_X_PH,REG_PID_X_IH,REG_PID_X_DH);
	pid_y_id = pid_manager_create_pid(REG_PID_Y_PH,REG_PID_Y_IH,REG_PID_Y_DH);
	pid_z_id = pid_manager_create_pid(REG_PID_Z_PH,REG_PID_Z_IH,REG_PID_Z_DH);

    reg_manager_connect_handler(REG_PID_X_UPDATE,pid_x_update);
    reg_manager_connect_handler(REG_PID_Y_UPDATE,pid_y_update);
    reg_manager_connect_handler(REG_PID_Z_UPDATE,pid_z_update);


    motor_manager_init();

    I2C_dev_init();

	sei();

    MPU6050_initialize();
    MPU6050_setXGyroOffset(100);
	MPU6050_setYGyroOffset(30);
	MPU6050_setZGyroOffset(50);


	while(1)
	{
        	reg_manager_update();
	        event_manager_update();
        	if(main_tick)
		{
			main_tick = 0;
			MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
			setpoints_calc(&thrust, &pitch, &roll, &yaw);

			pid_x_out = pid_manager_update_pid(pid_x_id,pitch,gx);
			pid_y_out = pid_manager_update_pid(pid_y_id,roll,gy);
			pid_z_out = pid_manager_update_pid(pid_z_id,yaw,gz);

			motor_manager_update(thrust, pid_x_out, pid_y_out,pid_z_out);
			if(reg_manager_get_reg(REG_LOG_ENABLE))
			{
				reg_manager_set_reg(REG_GYRO_XH,(uint8_t)(gx >> 8));
				reg_manager_set_reg(REG_GYRO_XL,(uint8_t)(gx));
				reg_manager_set_reg(REG_GYRO_YH,(uint8_t)(gy >> 8));
				reg_manager_set_reg(REG_GYRO_YL,(uint8_t)(gy));
				reg_manager_set_reg(REG_GYRO_ZH,(uint8_t)(gz >> 8));
				reg_manager_set_reg(REG_GYRO_ZL,(uint8_t)(gz));


				reg_manager_set_reg(REG_PID_XH,(uint8_t)(pid_x_out >> 8));
				reg_manager_set_reg(REG_PID_XL,(uint8_t)(pid_x_out));

				reg_manager_set_reg(REG_PID_YH,(uint8_t)(pid_y_out >> 8));
				reg_manager_set_reg(REG_PID_YL,(uint8_t)(pid_y_out));

				reg_manager_set_reg(REG_PID_ZH,(uint8_t)(pid_z_out >> 8));
				reg_manager_set_reg(REG_PID_ZL,(uint8_t)(pid_z_out));

			}
        }
	}
	return 0;
}
