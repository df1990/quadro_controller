#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "common.h"

char dupa[32] = "TMP_LOG_TEST\n\r";

int main(void)
{
	uart_init();
	DBG_LED0_INIT;
	DBG_LED1_INIT;
	DBG_LED2_INIT;
	DBG_LED3_INIT;
	DBG_LED0_OFF;
	DBG_LED1_OFF;
	DBG_LED2_OFF;
	DBG_LED3_OFF;
	sei();
	uart_send_frame((uint8_t*)(dupa),15);
	while(1)
	{
		if(1 == uart_frame_rx_complete())
		{
			DBG_LED3_TOGGLE;
			uart_get_frame((uint8_t*)(dupa));
			while(!uart_frame_tx_complete())
			{
				//wait
			}
			uart_send_frame((uint8_t*)(dupa+1),dupa[0]);
		}
	}

	return 0;
}
