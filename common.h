#ifndef _COMMON_H_
#define _COMMON_H

#define DBG_LED0_INIT DDRC |= (1<<PC0)
#define DBG_LED0_ON PORTC &= ~(1<<PC0)
#define DBG_LED0_OFF PORTC |= (1<<PC0)
#define DBG_LED0_TOGGLE (PINC = (1<<PC0))

#define DBG_LED1_INIT DDRC |= (1<<PC1)
#define DBG_LED1_ON PORTC &= ~(1<<PC1)
#define DBG_LED1_OFF PORTC |= (1<<PC1)
#define DBG_LED1_TOGGLE (PINC = (1<<PC1))

#define DBG_LED2_INIT DDRC |= (1<<PC2)
#define DBG_LED2_ON PORTC &= ~(1<<PC2)
#define DBG_LED2_OFF PORTC |= (1<<PC2)
#define DBG_LED2_TOGGLE (PINC = (1<<PC2))

#define DBG_LED3_INIT DDRC |= (1<<PC3)
#define DBG_LED3_ON PORTC &= ~(1<<PC3)
#define DBG_LED3_OFF PORTC |= (1<<PC3)
#define DBG_LED3_TOGGLE (PINC = (1<<PC3))

//UART FRAME FORMAT
//#define FRAME_LENGTH 		0
//#define FRAME_COMMAND 		1
//#define FRAME_REG_ADDR		2
//#define FRAME_REG_COUNT 	3
//#define FRAME_REG_VALUES 	4

//#define COMMAND_REG_WRITE	'w'
//#define COMMAND_REG_READ	'r'

//#define BASE_FRAME_LENGTH	4
//#define E_ADDR_INV	1
//#define E_CNT_INV	2


//COMMON DEF
#define FRAME_BUFFER_SIZE 32

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



#endif
