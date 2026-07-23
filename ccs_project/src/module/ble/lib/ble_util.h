#ifndef BLE_UTIL_H
#define BLE_UTIL_H

#include <stdint.h>
#include "util_tool.h"

#define DEF_WEAK_BLE_EVENT_HANDLER(event) ATTR_WEAK void handle_ ## event(uint32 event, BLEAppUtil_msgHdr_t* pMsgData) {;}
#define DEF_STATIC_BLE_EVENT_CFG(name, event, mask) \
    static BLEAppUtil_EventHandler_t name = {event, HANDLER_NAME(event), (mask)};

void ble_util_reverse_addr(const uint8_t* p_addr, uint8_t* p_rev_addr);

#endif
