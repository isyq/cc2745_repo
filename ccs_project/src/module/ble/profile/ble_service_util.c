#include "hal_ble_api.h"
#include "hal_os_memory.h"
#include "hal_ble_config.h"
#include "ble_service_util.h"
#include "ble_char.h"
#include "app_util.h"

bStatus_t BleService_gattNotify(uint16_t connHandle, uint16_t attHandle, uint8_t* pData, uint8_t dataLen)
{
    bStatus_t status = bleMemAllocError;
    attHandleValueNoti_t noti;

    noti.pValue = GATT_bm_alloc(connHandle, ATT_HANDLE_VALUE_NOTI, dataLen, NULL);
    if (noti.pValue != NULL)
    {
        noti.handle = attHandle;
        noti.len    = dataLen;
        memcpy(noti.pValue, pData, dataLen);

        if (GATT_Notification(connHandle, &noti, 0) != SUCCESS)
        {
            GATT_bm_free((gattMsg_t*)&noti, ATT_HANDLE_VALUE_NOTI);
        }
        else
        {
            status = SUCCESS;
        }        
    }

    return status;
}

bStatus_t BleService_postWriteCccdEvent(eventId_t eventId, uint16_t connHandle, bleChann_t channId, bool cccdEnabled)
{
    writeCccdEventData_t eventData = {
        .connHandle = connHandle,
        .channId    = channId,
        .cccdReady  = cccdEnabled,
    };

    return BleObs_enqueueCustom(eventId, &eventData, sizeof(writeCccdEventData_t));
}

bStatus_t BleService_postWriteCharEvent(eventId_t eventId, uint16_t connHandle, bleChann_t channId, uint8_t* pData,
                                        uint8_t dataLen)
{
    bStatus_t status = SUCCESS;

    uint8_t* pWriteData = MALLOC(dataLen);
    if (pWriteData != NULL)
    {
        /* Copy data from BLE stack to heap */
        memcpy(pWriteData, pData, dataLen);

        writeCharEventData_t eventData = {
            .connHandle = connHandle,
            .channId    = channId,
            .pData      = pWriteData,
            .dataLen    = dataLen,
        };

        status = BleObs_enqueueCustom(eventId, &eventData, sizeof(writeCharEventData_t));
    }

    return pWriteData == NULL ? bleMemAllocError : status;
}

bStatus_t BleService_initCharCfg(gattCharCfg_t** ppCCCD, uint8_t linkNumMax)
{
    gattCharCfg_t* pCharCfg = MALLOC_STATIC(sizeof(gattCharCfg_t) * linkNumMax);
    if (pCharCfg != NULL)
    {
        GATTServApp_InitCharCfg(BAD_CONN_HANDLE, pCharCfg);

        *ppCCCD = pCharCfg;
    }

    return pCharCfg == NULL ? bleMemAllocError : SUCCESS;
}

bStatus_t BleService_notify(uint16_t connHandle, uint16_t charHandle, uint8_t* pData, uint8_t dataLen, uint8_t authen)
{
    attHandleValueNoti_t noti;
    uint16 len;
    bStatus_t status;

    // If the attribute value is longer than (ATT_MTU - 3) octets, then
    // only the first (ATT_MTU - 3) octets of this attributes value can
    // be sent in a notification.
    noti.pValue = (uint8*)GATT_bm_alloc(connHandle, ATT_HANDLE_VALUE_NOTI, GATT_MAX_MTU, &len);
    if (noti.pValue != NULL)
    {
        noti.handle = charHandle;
        noti.len = GET_MIN(len, dataLen);
        memcpy(noti.pValue, pData, noti.len);

        status = GATT_Notification(connHandle, &noti, authen);

        /* Free memory if error occurs */
        if (status != SUCCESS)
        {
            GATT_bm_free((gattMsg_t*)&noti, ATT_HANDLE_VALUE_NOTI);
        }
    }
    else
    {
        status = bleNoResources;
    }

    return status;
}

bStatus_t BleService_indicate(uint16_t connHandle, uint16_t charHandle, uint8_t* pData, uint8_t dataLen, 
                              uint8_t authen, uint8_t taskId)
{
    attHandleValueNoti_t noti;
    uint16 len;
    bStatus_t status;

    // If the attribute value is longer than (ATT_MTU - 3) octets, then
    // only the first (ATT_MTU - 3) octets of this attributes value can
    // be sent in a notification.
    noti.pValue = (uint8*)GATT_bm_alloc(connHandle, ATT_HANDLE_VALUE_NOTI, GATT_MAX_MTU, &len);
    if (noti.pValue != NULL)
    {
        noti.handle = charHandle;
        noti.len = GET_MIN(len, dataLen);
        memcpy(noti.pValue, pData, noti.len);

        status = GATT_Indication(connHandle, (attHandleValueInd_t*)&noti, authen, taskId);

        /* Free memory if error occurs */
        if (status != SUCCESS)
        {
            GATT_bm_free((gattMsg_t*)&noti, ATT_HANDLE_VALUE_NOTI);
        }
    }
    else
    {
        status = bleNoResources;
    }

    return status;
}
