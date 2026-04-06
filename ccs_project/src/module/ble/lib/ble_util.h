#ifndef BLE_UTIL_H
#define BLE_UTIL_H

#include <stdint.h>

#define DEF_BLE_EVENT_HANDLER_NAME(event) event ## _handler

#define DEF_WEAK_BLE_EVENT_HANDLER(event) \
        __attribute__((__weak__))          \
        void event ## _handler(uint32 event, BLEAppUtil_msgHdr_t* pMsgData) {;}


void ble_util_reverse_addr(const uint8_t* p_addr, uint8_t* p_rev_addr);


#endif
