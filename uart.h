#ifndef _UART_H_
#define _UART_H_

#include <avr/io.h>
#include "fifo.h"

void uart_init(struct fifo *rx_f, struct fifo *tx_f);

void uart_send_tx_fifo(void);
//void uart_connect_event(void);

//uint8_t uart_frame_rx_complete(void);
//uint8_t uart_frame_tx_complete(void);
//uint8_t uart_get_frame(uint8_t *buffer);
//void uart_send_frame(uint8_t *buffer);

void uart_log(const char *string);
void uart_log_v(char *string);

#endif
