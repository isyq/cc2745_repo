#include "bleapputil_api.h"
#include "ti_ble_config.h"
#include "log.h"
#include "ble_util.h"

#ifndef CFG_ADV_SET_COUNT
#define CFG_ADV_SET_COUNT BLE_CONFIG_NUM_ADV_SETS
#endif

static uint8_t m_adv_handles[CFG_ADV_SET_COUNT];

static uint8_t get_adv_set_id(uint8_t adv_handle)
{
    for (uint8_t i = 0; i < CFG_ADV_SET_COUNT; i++)
    {
        if (m_adv_handles[i] == adv_handle)
        {
            return i;
        }
    }

    return 0xFF;
}

static void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_ADV_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* pMsgData)
{
    uint8_t adv_handle = ((BLEAppUtil_AdvEventData_t*)pMsgData)->pBuf->advHandle;
    uint8_t set_id     = get_adv_set_id(adv_handle);

    if (set_id == 0xFF)
    {
        log_error("Adv event handle not found");
        return;
    }

    switch (event)
    {
    case BLEAPPUTIL_ADV_START_AFTER_ENABLE:
        log_info("Adv started: %d", set_id);
        break;

    case BLEAPPUTIL_ADV_END_AFTER_DISABLE:
        log_info("Adv stopped: %d", set_id);
        break;

    default:
        break;
    }
}

DEF_STATIC_BLE_EVENT_CFG(m_adv_event_cfg, BLEAPPUTIL_GAP_ADV_TYPE,
                         BLEAPPUTIL_ADV_START_AFTER_ENABLE |
                         BLEAPPUTIL_ADV_END_AFTER_DISABLE)

void ble_adv_init(void)
{
    BLEAppUtil_registerEventHandler(&m_adv_event_cfg);
    BleConfig_initAdvSets(m_adv_handles, NULL);
}

void ble_adv_start(void)
{
    log_info("Start advs");

    BleConfig_startAdvSets(m_adv_handles, NULL, CFG_ADV_SET_COUNT);
}

void ble_adv_stop(void)
{
    BleConfig_stopAdvSets(m_adv_handles, NULL, CFG_ADV_SET_COUNT);
}

void ble_adv_update(uint8_t set_id, uint8_t* p_data, uint8_t len)
{
    log_info("Set adv data: %d", set_id);

    bStatus_t status = GapAdv_prepareLoadByHandle(m_adv_handles[set_id], GAP_ADV_FREE_OPTION_DONT_FREE);

    if (status == SUCCESS)
    {
        GapAdv_loadByHandle(m_adv_handles[set_id], GAP_ADV_DATA_TYPE_ADV, sizeof(advData1), advData1);
    }
    else
    {
        log_error("Adv data update failed: %d", status);
    }
}
