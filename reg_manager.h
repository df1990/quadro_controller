#ifndef _REG_MANAGER_H_
#define _REG_MANAGER_H_
#include <avr/io.h>

//REG DEFINITIONS
#define REG_COUNT 35

#define REG_STATUS 	0
#define REG_COMMAND 	1

#define REG_THRUST	2
#define REG_PITCH	3
#define REG_ROLL	4
#define REG_YAW		5

#define REG_PID_X_PH		6//-
#define REG_PID_X_PL		7//-
#define REG_PID_X_IH		8//-- PID X
#define REG_PID_X_IL		9//-- PID X
#define REG_PID_X_DH		10//-
#define REG_PID_X_DL		11//-

#define REG_PID_Y_PH		12//-
#define REG_PID_Y_PL		13//-
#define REG_PID_Y_IH		14//-- PID Y
#define REG_PID_Y_IL		15//-- PID Y
#define REG_PID_Y_DH		16//-
#define REG_PID_Y_DL		17//-

#define REG_PID_Z_PH		18//-
#define REG_PID_Z_PL		19//-
#define REG_PID_Z_IH		20//-- PID Z
#define REG_PID_Z_IL		21//-- PID Z
#define REG_PID_Z_DH		22//-
#define REG_PID_Z_DL		23//-

#define REG_LED			24

#define REG_FL_PWM		25
#define REG_FR_PWM		26
#define REG_BL_PWM		27
#define REG_BR_PWM		28

#define REG_FR_BL_ENABLE	29
#define REG_FL_BR_ENABLE	30

#define REG_GX_OFFSET		31
#define REG_GY_OFFSET		32
#define REG_GZ_OFFSET		33

#define REG_LOG1_ENABLE		34
//REG DEFINITIONS END

void reg_manager_init(void);
void reg_manager_update(void);
uint8_t reg_manager_get_reg(uint8_t reg_id);
void reg_manager_set_reg(uint8_t reg_id, uint8_t reg_value);
void reg_manager_connect_handler(uint8_t reg_id, void (*handler)(uint8_t));
#endif
