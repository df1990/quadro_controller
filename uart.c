#include <avr/interrupt.h>
#include <string.h>
#include "uart.h"
#include "common.h"

#define UART_BUFF_SIZE 32

uint8_t uart_rx_buff[UART_BUFF_SIZE];
volatile uint8_t uart_rx_buff_cnt;
volatile uint8_t uart_rx_complete;

uint8_t uart_tx_buff[UART_BUFF_SIZE];
volatile uint8_t uart_tx_buff_cnt;
volatile uint8_t uart_tx_complete;
//frame format
//<LENGTH><COMMAND><REG_ADDR><REG_COUNT>[<REG_VALUES>]

void uart_init(void)
{

	uart_rx_buff_cnt = 0;
	uart_rx_complete = 0;

	uart_tx_buff_cnt = 0;
	uart_tx_complete = 1;

	//Set baud rate - 57600 baud rate + double speed mode
	UBRR0H = (uint8_t)(34>>8);
	UBRR0L = (uint8_t)(34);
	UCSR0A = (1<<U2X0);
	//Set frame format: 8data, No parity, 1stop bit
	UCSR0C = (3<<UCSZ00);
	//Enable receiver and transmitter
	UCSR0B = ((1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0));


}

uint8_t uart_frame_rx_complete(void)
{
	return uart_rx_complete;
	//return 0;
}

uint8_t uart_frame_tx_complete(void)
{
	return uart_tx_complete;
	//return 0;
}

uint8_t uart_get_frame(uint8_t *buffer)
{
	if(uart_rx_complete)
	{
		memcpy(buffer,uart_rx_buff,uart_rx_buff_cnt);
		uart_rx_complete = 0;
		uart_rx_buff_cnt = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t uart_send_frame(uint8_t *buffer, uint8_t len)
{
	if(uart_tx_complete)
	{
		uart_tx_buff[0] = len;
		memcpy(uart_tx_buff+1,buffer,len);
		uart_tx_complete = 0;
		uart_tx_buff_cnt = 0;
		UCSR0B |= (1 << UDRIE0);
		return 1;
	}
	else
	{	
		return 0;
	}
}

ISR(USART_RX_vect)
{
	DBG_LED1_TOGGLE;
	uart_rx_buff[uart_rx_buff_cnt] = UDR0;
	uart_rx_buff_cnt++;
	if(uart_rx_buff[0] == uart_rx_buff_cnt)
	{
		uart_rx_complete = 1;
	}
}

ISR(USART_TX_vect)
{

}

ISR(USART_UDRE_vect)
{
	DBG_LED0_TOGGLE;
	UDR0 = uart_tx_buff[uart_tx_buff_cnt];
	uart_tx_buff_cnt++;
	if(uart_tx_buff[0] == uart_tx_buff_cnt)
	{
		uart_tx_complete = 1;
		UCSR0B &= ~(1 << UDRIE0);
	}
}
