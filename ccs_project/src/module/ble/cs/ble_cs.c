#include "ranging/ranging_profile.h"
#include "ti_ble_config.h"
#include "bleapputil_api.h"
#include "ble_util.h"
#include "log.h"

#ifndef MAX_NUM_BLE_CONNS
#define MAX_NUM_BLE_CONNS 4
#endif

#define CS_REAL_TIME_MODE 0

DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_CS_TYPE);

static BLEAppUtil_EventHandler_t m_cs_event_cfg =
{
    .handlerType   = BLEAPPUTIL_CS_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_CS_TYPE),
    .eventMask     = BLEAPPUTIL_CS_EVENT_CODE
};

static uint32_t gProcedureCounter[MAX_NUM_BLE_CONNS];
static uint8_t gProcedureAntennaPath[MAX_NUM_BLE_CONNS];

static void update_cccd_status(uint16_t conn_handle, uint16_t p_value)
{
    return;
}

static void update_proc_status(uint8_t status, uint16_t conn_handle, uint16_t ranging_counter)
{
    switch (status)
    {
    case RRSP_SENDING_PROCEDURE_STARTED:
        break;

    case RRSP_SENDING_PROCEDURE_ENDED:
        break;

    case RRSP_STATUS_SENDING_PROCEDURE_ABORTED:
        break;

    default:
        break;
    }
}

static RRSP_cb_t m_profile_cb =
{
    update_cccd_status,
    update_proc_status,
};

void ble_cs_init(void)
{
    BLEAppUtil_registerEventHandler(&m_cs_event_cfg);
    BLEAppUtil_registerCsCB();

    memset(gProcedureCounter, 0xFF, MAX_NUM_BLE_CONNS * sizeof(gProcedureCounter[0]));
    memset(gProcedureAntennaPath, 0, MAX_NUM_BLE_CONNS * sizeof(gProcedureAntennaPath[0]));

    RRSP_start(&m_profile_cb, CS_REAL_TIME_MODE);
}
