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

#endif
