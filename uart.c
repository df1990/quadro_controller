#include <avr/interrupt.h>
#include <string.h>
#include "uart.h"
#include "common.h"
#include "event_manager.h"
#include "config.h"

//#define UART_BUFF_SIZE 64

//uint8_t uart_rx_buff[UART_BUFF_SIZE];
//volatile uint8_t uart_rx_buff_cnt;
//volatile uint8_t uart_rx_complete;

//uint8_t uart_tx_buff[UART_BUFF_SIZE];
//volatile uint8_t uart_tx_buff_cnt;
//volatile uint8_t uart_tx_complete;
//frame format
//<LENGTH><COMMAND><REG_ADDR><REG_COUNT>[<REG_VALUES>]

//static uint8_t frame_timeout_event_id;
//volatile uint8_t tmp;

struct fifo *rx_fifo;
struct fifo *tx_fifo;

void uart_init(struct fifo *rx_f, struct fifo *tx_f)
{

	rx_fifo = rx_f;
	tx_fifo = tx_f;

	//Set baud rate - 57600 baud rate + double speed mode
	UBRR0H = (uint8_t)(34>>8);
	UBRR0L = (uint8_t)(34);
	UCSR0A = (1<<U2X0);
	//Set frame format: 8data, No parity, 1stop bit
	UCSR0C = (3<<UCSZ00);
	//Enable receiver and transmitter
	UCSR0B = ((1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0));

}

void uart_send_tx_fifo(void)
{
	UCSR0B |= (1 << UDRIE0);
}

void uart_log_nl(void)
{
#ifdef DEBUG
	while(!(UCSR0A & (1<<UDRE0)));//wait
	UDR0 = '\n';
	while(!(UCSR0A & (1<<UDRE0)));//wait
	UDR0 = '\r';
#endif
}

void uart_log(const char *string)
{
#ifdef DEBUG
	uart_log_v((char *)string);
#endif
}

void uart_log_v(char *string)
{
#ifdef DEBUG
	char *p;
	p = string;
	while(*p)
	{
		DBG_LED1_TOGGLE;
		while(!(UCSR0A & (1<<UDRE0)));//wait
		UDR0 = (uint8_t)(*p++);
	}
	uart_log_nl();
#endif
}

ISR(USART_RX_vect)
{
	DBG_LED0_TOGGLE;
	fifo_put(rx_fifo,UDR0);
}

ISR(USART_TX_vect)
{

}

ISR(USART_UDRE_vect)
{
	DBG_LED1_TOGGLE;
	uint8_t tmp;
	if(fifo_get(tx_fifo,&tmp))
	{
		UDR0 = tmp;
	}
	else
	{
		UCSR0B &= ~(1 << UDRIE0);
	}
}
