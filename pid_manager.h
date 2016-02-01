#ifndef _PID_MANAGER_H_
#define _PID_MANAGER_H_

#include <avr/io.h>
#include "common.h"

#define PID_COUNT 3

void pid_manager_init(void);
uint8_t pid_manager_create_pid(uint8_t pid_p_addr, uint8_t pid_i_addr, uint8_t pid_d_addr);
uint8_t pid_manager_reinit_pid(uint8_t pid_id);
void pid_manager_reset_pid(uint8_t pid_id);
int16_t pid_manager_update_pid(uint8_t pid_id, int16_t set_val, int16_t meas_val);

#endif
