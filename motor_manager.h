#ifndef _MOTOR_MANAGER_H_
#define _MOTOR_MANAGER_H_

#include <avr/io.h>

void motor_manager_init(void);
void motor_manager_update(int16_t thrust, int16_t pid_x, int16_t pid_y, int16_t pid_z);

//DBG_ONLY
//void motor_manager_pwm_set(uint8_t fl, uint8_t fr, uint8_t bl, uint8_t br);

#endif
