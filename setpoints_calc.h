#ifndef _SETPOINTS_CALC_H_
#define _SETPOINTS_CALC_H_

#include <avr/io.h>

#define GYRO_MAX_RANGE 250L
#define MAX_INT16 32767L
#define MIN_INT16 -32768L

#define PITCH_MAX_RANGE 30L //max pitch rotation speed in deg/sec
#define ROLL_MAX_RANGE 30L //max roll rotation speed in deg/sec
#define YAW_MAX_RANGE 30L //max yaw rotation speed in deg/sec

#define THRUST_MAX_VALUE MAX_INT16
#define THRUST_MIN_VALUE 0

#define PITCH_MAX_VALUE ((PITCH_MAX_RANGE * MAX_INT16)/GYRO_MAX_RANGE)
#define PITCH_MIN_VALUE (-PITCH_MAX_VALUE)

#define ROLL_MAX_VALUE ((ROLL_MAX_RANGE * MAX_INT16)/GYRO_MAX_RANGE)
#define ROLL_MIN_VALUE (-ROLL_MAX_VALUE)

#define YAW_MAX_VALUE ((YAW_MAX_RANGE * MAX_INT16)/GYRO_MAX_RANGE)
#define YAW_MIN_VALUE (-YAW_MAX_VALUE)

//void setpoints_init(uint8_t thrust_reg, uint8_t pitch_reg, uint8_t roll_reg, uint8_t yaw_reg);
void setpoints_calc(int16_t *thrust, int16_t *pitch, int16_t *roll, int16_t *yaw);

#endif

