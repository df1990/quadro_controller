#ifndef _COMMON_H_
#define _COMMON_H_

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

//COMMON DEF
#define FRAME_BUFFER_SIZE 32



inline void dbg_led0_blink(uint8_t value)
{
	if(value)DBG_LED0_ON;
	else DBG_LED0_OFF;
}

inline void dbg_led1_blink(uint8_t value)
{
	if(value)DBG_LED1_ON;
	else DBG_LED1_OFF;
}

inline void dbg_led2_blink(uint8_t value)
{
	if(value)DBG_LED2_ON;
	else DBG_LED2_OFF;
}

inline void dbg_led3_blink(uint8_t value)
{
	if(value)DBG_LED3_ON;
	else DBG_LED3_OFF;
}



#endif
