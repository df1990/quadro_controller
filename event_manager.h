#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_
#include <avr/io.h>

enum event_type {EVENT_SINGLE, EVENT_CONTINOUS};

void event_manager_init(void);
uint8_t event_manager_connect_event(uint32_t tick_count, void (*event)(void), enum event_type type);
void event_manager_update(void);
void event_manager_start_event(uint8_t event_id);
void event_manager_stop_event(uint8_t event_id);
void event_manager_reinit_event(uint8_t event_id, uint32_t tick_count, enum event_type type);

#endif
