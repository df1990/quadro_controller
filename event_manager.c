#include "event_manager.h"
#include "common.h"
#include <avr/interrupt.h>
#include <stdio.h>

uint8_t tick;
uint32_t tick_count; 


struct event_slot
{
	uint8_t slot_connected;
	uint8_t event_active;
	enum event_type type;
	uint32_t ticks_left;
	uint32_t ticks_interval;
	void (*event)(void);	
};

#define SLOTS_COUNT 10
struct event_slot slots[SLOTS_COUNT];


void event_manager_init(void)
{
	tick = 0;
	tick_count = 0;
	//
	uint8_t index;
	for(index = 0; index < SLOTS_COUNT; index++)
	{
		slots[index].slot_connected = 0;
		slots[index].event_active = 0;
		slots[index].type = EVENT_SINGLE;
		slots[index].ticks_left = 0;
		slots[index].ticks_interval = 0;
		slots[index].event = NULL;
	}

	//
	TCCR2A = (1<<WGM21);//CTC mode
	TCCR2B = ((1<<CS22)|(0<<CS21)|(0<<CS20));//Prescaler 64
	OCR2A = 248;//1kHZ interrupt generation frequency
	TIMSK2 |= (1<<OCIE2A);
}


uint8_t event_manager_connect_event(uint32_t tick_count, void (*event)(void), enum event_type type)
{
	uint8_t index;
	for(index = 0; index < SLOTS_COUNT; index++)
	{
		if(0 == slots[index].slot_connected)
		{
			slots[index].slot_connected = 1;
			slots[index].type = type;
			if(type == EVENT_CONTINOUS)
			{
				slots[index].event_active = 1;
				slots[index].ticks_left = tick_count;
			}
			else
			{
				slots[index].event_active = 0;
				slots[index].ticks_left = 0;
			}
			slots[index].ticks_interval = tick_count;
			slots[index].event = event;
			return (index + 1);
		}
	}
	return 0;
}

void event_manager_update(void)
{
	if(tick)
	{
		tick = 0;
		uint8_t index;
		for(index = 0; index < SLOTS_COUNT; index++)
		{
			if((1 == slots[index].slot_connected) &&
			   (1 == slots[index].event_active))
			{
				slots[index].ticks_left = slots[index].ticks_left - 1;
				if(0 == slots[index].ticks_left)
				{
					slots[index].event();
				
					if(slots[index].type == EVENT_CONTINOUS)
					{
						slots[index].ticks_left = slots[index].ticks_interval;
					}
					else
					{
						slots[index].event_active = 0;
					}
				}
			}
		}
	}
}

void event_manager_start_event(uint8_t event_id)
{
	if((event_id > 0) && (event_id <= SLOTS_COUNT))
	{
		uint8_t event_index = event_id -1;
		if(1 == slots[event_index].slot_connected)
		{
			slots[event_index].event_active = 1;
			slots[event_index].ticks_left = slots[event_index].ticks_interval;
		}
	}
}

void event_manager_stop_event(uint8_t event_id)
{
	if((event_id > 0) && (event_id <= SLOTS_COUNT))
	{
		uint8_t event_index = event_id -1;
		if(1 == slots[event_index].slot_connected)
		{
			slots[event_index].event_active = 0;
			slots[event_index].ticks_left = 0;
		}
	}
}

ISR(TIMER2_COMPA_vect)
{
	//DBG_LED2_TOGGLE;
	tick = 1;
	tick_count++;
}
