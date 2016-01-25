#ifndef _MOTOR_MANAGER_H_
#define _MOTOR_MANAGER_H_

#include <avr/io.h>

void motor_manager_init(void);
void motor_manager_update(int16_t thrust, int16_t pid_x, int16_t pid_y, int16_t pid_z);

#endif
