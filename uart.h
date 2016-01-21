#ifndef _UART_H_
#define _UART_H_

#include <avr/io.h>

void uart_init(void);


uint8_t uart_frame_rx_complete(void);
uint8_t uart_frame_tx_complete(void);
uint8_t uart_get_frame(uint8_t *buffer);
uint8_t uart_send_frame(uint8_t *buffer, uint8_t len);

#endif
