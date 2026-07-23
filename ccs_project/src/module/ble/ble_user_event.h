#ifndef BLE_USER_EVENT_H
#define BLE_USER_EVENT_H

#include <stdint.h>
#include "util_tool.h"

#define BLE_USER_EVENT_STACK_READY   0
#define BLE_USER_EVENT_IN_CONNECTION 1
#define BLE_USER_EVENT_NO_CONNECTION 2

#define BLE_USER_EVENT_COUNT         (BLE_USER_EVENT_NO_CONNECTION + 1)

#define DEF_WEAK_BLE_USER_EVENT_HANDLER(name) ATTR_WEAK void name(char* p_data) {;}
#define DECL_BLE_USER_EVENT_HANDLER(name) void name(char* p_data);

typedef struct
{
    uint16_t data_len;
    uint8_t p_data[];
} ble_user_data_t;

void ble_user_event_start(uint8_t event, uint8_t* p_data, uint16_t data_len);
void ble_user_event_stop(ble_user_data_t* p_event);

DECL_BLE_USER_EVENT_HANDLER(HANDLER_NAME(BLE_USER_EVENT_INIT_STACK))
DECL_BLE_USER_EVENT_HANDLER(HANDLER_NAME(BLE_USER_EVENT_STACK_READY))
DECL_BLE_USER_EVENT_HANDLER(HANDLER_NAME(BLE_USER_EVENT_IN_CONNECTION))
DECL_BLE_USER_EVENT_HANDLER(HANDLER_NAME(BLE_USER_EVENT_NO_CONNECTION))

#endif
