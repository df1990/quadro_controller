#include "motor_manager.h"
#include "event_manager.h"
#include "reg_manager.h"
#include "setpoints_calc.h"
#include "uart.h"
#include "common.h"
#include <avr/interrupt.h>

#define TIM0_TIM1_START (GTCCR &= (~(1 << PSRSYNC)))
#define TIM0_TIM1_STOP (GTCCR |= (1 << PSRSYNC))

void pwm_start_event(void)
{
	TIM0_TIM1_START;
}

#define FL_MASK (1 << 0)
#define FR_MASK (1 << 1)
#define BL_MASK (1 << 2)
#define BR_MASK (1 << 3)

#define MOTOR_FL_ENABLE (DDRD |= (1 << PD6))
#define MOTOR_FR_ENABLE (DDRD |= (1 << PD5))
#define MOTOR_BL_ENABLE (DDRB |= (1 << PB1))
#define MOTOR_BR_ENABLE (DDRB |= (1 << PB2))

#define MOTOR_FL_DISABLE (DDRD &= ~(1 << PD6))
#define MOTOR_FR_DISABLE (DDRD &= ~(1 << PD5))
#define MOTOR_BL_DISABLE (DDRB &= ~(1 << PB1))
#define MOTOR_BR_DISABLE (DDRB &= ~(1 << PB2))

void motor_manager_pwm_set(uint8_t fl, uint8_t fr, uint8_t bl, uint8_t br)
{
	OCR0A = 255 - fl;
	OCR0B = 255 - fr;
	OCR1A = 255 - bl;
	OCR1B = 255 - br;
}

void motor_enable_update(uint8_t mask)
{
    if(mask & FL_MASK)
    {
        MOTOR_FL_ENABLE;
    }
    else
    {
        MOTOR_FL_DISABLE;
    }
    /////////////////////////////
    if(mask & FR_MASK)
    {
        MOTOR_FR_ENABLE;
    }
    else
    {
        MOTOR_FR_DISABLE;
    }
    /////////////////////////////
    if(mask & BL_MASK)
    {
        MOTOR_BL_ENABLE;
    }
    else
    {
        MOTOR_BL_DISABLE;
    }
    /////////////////////////////
    if(mask & BR_MASK)
    {
        MOTOR_BR_ENABLE;
    }
    else
    {
        MOTOR_BR_DISABLE;
    }
}

void dbg_pwm_update(uint8_t value)
{
    if(value)
    {
        motor_manager_pwm_set(reg_manager_get_reg(REG_FL_PWM), reg_manager_get_reg(REG_FR_PWM), reg_manager_get_reg(REG_BL_PWM), reg_manager_get_reg(REG_BR_PWM));
    }
}

void motor_manager_init(void)
{

    //PWM PINS
	motor_enable_update(0);
	//DDRD |= ((1<<PD5)|(1<<PD6));
    //DDRB |= ((1<<PB1)|(1<<PB2));

	//TIMER 0 INIT
	TCCR0A = ((1<<COM0A1)|(1<<COM0A0)|(1<<COM0B1)|(1<<COM0B0)|(1<<WGM00));//Phase Correct PWM, output A inverting, output B inverting
	OCR0A = 255;
	OCR0B = 255;
	TIMSK0 = (1<<TOIE0);
	TCCR0B=((1<<CS01)|(1<<CS00));//Timer Start, prescaler 64

	//TIMER 1 INIT
	TCCR1A = ((1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0)|(1<<WGM10));//Phase Correct PWM, output A inverting, output B inverting
	OCR1A = 255;
	OCR1B = 255;
	TCCR1B = ((1<<CS11)|(1<<CS10));//Timer Start, prescaler 64

	GTCCR |= (1 << TSM);

	event_manager_connect_event(3,pwm_start_event,EVENT_CONTINOUS);
	reg_manager_connect_handler(REG_MOTOR_ENABLE,motor_enable_update);
	reg_manager_connect_handler(REG_PWM_UPDATE,dbg_pwm_update);
}



void motor_manager_update(int16_t thrust, int16_t pid_x, int16_t pid_y, int16_t pid_z)
{
	int32_t pwm_fr, pwm_fl, pwm_br, pwm_bl;
	if(thrust >= ((THRUST_MAX_VALUE/100)*5))//5% of total range
	{
		pwm_fr = (int32_t)thrust + (int32_t)pid_x - (int32_t)pid_y + (int32_t)pid_z;
		pwm_fl = (int32_t)thrust + (int32_t)pid_x + (int32_t)pid_y - (int32_t)pid_z;
		pwm_br = (int32_t)thrust - (int32_t)pid_x - (int32_t)pid_y - (int32_t)pid_z;
		pwm_bl = (int32_t)thrust - (int32_t)pid_x + (int32_t)pid_y + (int32_t)pid_z;

		pwm_fr = (pwm_fr > 32767) ? 32767 : ((pwm_fr < 0) ? 0 : pwm_fr);
		pwm_fl = (pwm_fl > 32767) ? 32767 : ((pwm_fl < 0) ? 0 : pwm_fl);
		pwm_br = (pwm_br > 32767) ? 32767 : ((pwm_br < 0) ? 0 : pwm_br);
		pwm_bl = (pwm_bl > 32767) ? 32767 : ((pwm_bl < 0) ? 0 : pwm_bl);

		pwm_fr = (pwm_fr >> 8);//[0~32767] to [0~127] range conversion
		pwm_fl = (pwm_fl >> 8);//[0~32767] to [0~127] range conversion
		pwm_br = (pwm_br >> 8);//[0~32767] to [0~127] range conversion
		pwm_bl = (pwm_bl >> 8);//[0~32767] to [0~127] range conversion

		pwm_fr += 127;
		pwm_fl += 127;
		pwm_br += 127;
		pwm_bl += 127;
	}
	else
	{

		pwm_fr = 10;
		pwm_fl = 10;
		pwm_br = 10;
		pwm_bl = 10;
	}

	motor_manager_pwm_set((uint8_t)pwm_fl,(uint8_t)pwm_fr,(uint8_t)pwm_bl,(uint8_t)pwm_br);

}

ISR(TIMER0_OVF_vect)
{
	TIM0_TIM1_STOP;
}
