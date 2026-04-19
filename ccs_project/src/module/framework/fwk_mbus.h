#ifndef FWK_MBUS_H
#define FWK_MBUS_H

#include <stdint.h>

#define FWK_EVENT_INIT_STACK  0
#define FWK_EVENT_STACK_READY 1
#define FWK_EVENT_ENTER_SLEEP 2
#define FWK_EVENT_EXIT_SLEEP  3

#define FWK_EVENT_COUNT       (FWK_EVENT_EXIT_SLEEP + 1)

void fwk_mbus_init(void);
void fwk_mbus_post(uint8_t event);
void fwk_mbus_pend(void);

#endif
