#ifndef BLE_SERVICE_TCAM_H
#define BLE_SERVICE_TCAM_H

#include "hal_ble_def.h"
#include "event_observer.h"
#include "ble_char.h"

bStatus_t Tcam_addService(uint8_t linkNumMax, eventHandler_t writeCharCB, eventHandler_t writeCccdCB);
bStatus_t Tcam_notifyToDkAuth(uint16_t connHandle, uint8_t* pData, uint8_t dataLen);
bStatus_t Tcam_notifyToDkAuxi(uint16_t connHandle, uint8_t* pData, uint8_t dataLen);
bStatus_t Tcam_notifyToCharger(uint16_t connHandle, uint8_t* pData, uint8_t dataLen);
bStatus_t Tcam_notify(uint16_t connHandle, bleChann_t channId, uint8_t* pData, uint8_t dataLen);

#endif
