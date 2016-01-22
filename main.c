#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "reg_manager.h"
#include "uart.h"
#include "common.h"
#include "event_manager.h"
#include "I2Cdev.h"
#include "MPU6050.h"
//char dupa[32] = "TMP_LOG_TEST\n\r";

uint8_t frame_buffer[32];

uint8_t main_tick;

void dbg_led_blink(void)
{
	//DBG_LED3_TOGGLE;
}

void dbg_led0_blink(void)
{
	DBG_LED0_TOGGLE;
}

void dbg_led1_blink(void)
{
	DBG_LED1_TOGGLE;
}

void dbg_led2_blink(void)
{
	DBG_LED2_TOGGLE;
}

void dbg_led3_blink(void)
{
	DBG_LED3_TOGGLE;
}

void main_loop_tick(void)
{
	main_tick = 1;
}


int main(void)
{
	
	int16_t ax, ay, az, gx, gy, gz;
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

	event_manager_init();
	reg_manager_init();
	uart_init();
	I2C_dev_init();

	event_manager_connect_event(1000,dbg_led_blink,EVENT_CONTINOUS);
	event_manager_connect_event(100,main_loop_tick,EVENT_CONTINOUS);
	
	reg_manager_connect_handler(REG_THRUST,dbg_led0_blink);
	reg_manager_connect_handler(REG_PITCH,dbg_led1_blink);
	reg_manager_connect_handler(REG_ROLL,dbg_led2_blink);
	reg_manager_connect_handler(REG_YAW,dbg_led3_blink);

	sei();

	MPU6050_initialize();
	//uart_send_frame((uint8_t*)(dupa),15);
	while(1)
	{
		event_manager_update();
		
		if(1 == uart_frame_rx_complete())
		{
			uart_get_frame(frame_buffer);
			if(reg_manager_parse(frame_buffer))
			{
				while(!uart_frame_tx_complete())
				{
					//wait
				}
				uart_send_frame(frame_buffer);
			}	
		}
		
		if(main_tick)
		{
			main_tick = 0;
			MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
			
			//uint8_t str_len;
			//str_len = sprintf((char*)(frame_buffer),"tax=%d ay=%d\n\r",ax,ay);
			//while(!uart_frame_tx_complete())
		//	{
				//wait
		//	}
			//uart_send_frame(frame_buffer,str_len);
			
		}
	}

	return 0;
}
