#ifndef FWK_EVENT_H
#define FWK_EVENT_H

#include <stdint.h>

#define FWK_EVENT_INIT_STACK  0
#define FWK_EVENT_STACK_READY 1
#define FWK_EVENT_ENTER_SLEEP 2
#define FWK_EVENT_EXIT_SLEEP  3

#define FWK_EVENT_COUNT       (FWK_EVENT_EXIT_SLEEP + 1)

void fwk_event_init(void);
void fwk_event_post(uint8_t event);
void fwk_event_pend(void);

#endif
