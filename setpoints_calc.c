#include "setpoints_calc.h"
#include "reg_manager.h"

//static uint8_t thrust_reg_id, pitch_reg_id, roll_reg_id, yaw_reg_id;

int16_t map(int16_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
	return (int16_t)(((int32_t)x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

/*void setpoints_init(uint8_t thrust_reg, uint8_t pitch_reg, uint8_t roll_reg, uint8_t yaw_reg)
{
    thrust_reg_id = thrust_reg;
    pitch_reg_id = pitch_reg;
    roll_reg_id = roll_reg;
    yaw_reg_id = yaw_reg;
}*/

void setpoints_calc(int16_t *thrust, int16_t *pitch, int16_t *roll, int16_t *yaw)
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
