#ifndef _FRAME_MANAGER_H_
#define _FRAME_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>

//UART FRAME FORMAT
//#define FRAME_LENGTH 		0
//#define FRAME_COMMAND 		1
//#define FRAME_REG_ADDR		2
//#define FRAME_REG_COUNT 	3
//#define FRAME_REG_VALUES 	4

#define COMMAND_REG_WRITE	'w'
#define COMMAND_REG_READ	'r'

//#define BASE_FRAME_LENGTH	4


#define ERR_CMD_INV     1
#define ERR_ADDR_INV	2
#define ERR_CNT_INV	    3



//<$><len><cmd><addr><count><#>[<values><crc>]
#define FRAME_NO_PAYLOAD_LENGTH 7
#define FRAME_HEADER_LENGTH 6
#define HEADER_START_CODE '$'
#define HEADER_END_CODE '#'
#define FRAME_END_CODE '@'

#define FRAME_START         0
#define FRAME_LENGTH        1
#define FRAME_CMD           2
#define FRAME_REG_ADDR      3
#define FRAME_REG_COUNT     4
#define FRAME_HEADER_END    5
#define FRAME_REG_VALUES    6

struct frame {
    uint8_t length;
	uint8_t command;
	uint8_t reg_addr;
	uint8_t reg_count;
	uint8_t reg_values[32];
};

void frame_manager_init(void);
uint8_t frame_manager_get_frame(struct frame *f);
uint8_t frame_manager_send_frame(struct frame *f);

void frame_manager_DBG(void);

#endif
