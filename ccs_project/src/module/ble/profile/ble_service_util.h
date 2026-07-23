#ifndef BLE_SERVICE_UTIL_H
#define BLE_SERVICE_UTIL_H

#include "hal_ble_api.h"
#include "ble_observer.h"
#include "ble_char.h"

bStatus_t BleService_gattNotify(uint16_t connHandle, uint16_t attHandle, uint8_t* pData, uint8_t dataLen);
bStatus_t BleService_postWriteCccdEvent(eventId_t eventId, uint16_t connHandle, bleChann_t channId,
                                        bool cccdEnabled);
bStatus_t BleService_postWriteCharEvent(eventId_t eventId, uint16_t connHandle, bleChann_t channId,
                                        uint8_t* pData, uint8_t dataLen);
bStatus_t BleService_initCharCfg(gattCharCfg_t** ppCCCD, uint8_t linkNumMax);
bStatus_t BleService_notify(uint16_t connHandle, uint16_t charHandle, uint8_t* pData, uint8_t dataLen,
                            uint8_t authen);

#endif
