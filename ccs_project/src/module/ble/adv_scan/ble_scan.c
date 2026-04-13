#include "ti_ble_config.h"
#include "bleapputil_api.h"
#include "ble_util.h"

const BLEAppUtil_ConnParams_t centralConnInitParams =
{
    .initPhys        = DEFAULT_INIT_PHY,
    .scanInterval    = INIT_PHYPARAM_SCAN_INT,            /* Units of 0.625ms */
    .scanWindow      = INIT_PHYPARAM_SCAN_WIN,            /* Units of 0.625ms */
    .minConnInterval = INIT_PHYPARAM_MIN_CONN_INT,        /* Units of 1.25ms  */
    .maxConnInterval = INIT_PHYPARAM_MAX_CONN_INT,        /* Units of 1.25ms  */
    .connLatency     = INIT_PHYPARAM_CONN_LAT,
    .supTimeout      = INIT_PHYPARAM_SUP_TO               /* Units of 10ms */
};

const BLEAppUtil_ScanInit_t centralScanInitParams =
{
    .primPhy         = DEFAULT_SCAN_PHY,
    .scanType        = DEFAULT_SCAN_TYPE,
    .scanInterval    = DEFAULT_SCAN_INTERVAL,
    .scanWindow      = DEFAULT_SCAN_WINDOW,
    .advReportFields = ADV_RPT_FIELDS,
    .scanPhys        = DEFAULT_INIT_PHY,
    .fltPolicy       = SCANNER_FILTER_POLICY,
    .fltPduType      = SCANNER_FILTER_PDU_TYPE,
    .fltMinRssi      = SCANNER_FILTER_MIN_RSSI,
    .fltDiscMode     = SCANNER_FILTER_DISC_MODE,
    .fltDup          = SCANNER_DUPLICATE_FILTER
};

BLEAppUtil_ConnectParams_t centralConnParams =
{
    .phys    = DEFAULT_INIT_PHY,
    .timeout = 0
};

static void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_SCAN_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* pMsgData)
{
    // BLEAppUtil_ScanEventData_t* scanMsg = (BLEAppUtil_ScanEventData_t*)pMsgData;

    switch (event)
    {
    case BLEAPPUTIL_SCAN_ENABLED:
    {
    }
    break;

    case BLEAPPUTIL_SCAN_DISABLED:
    {
    }
    break;

    default:
        break;

    }
}

void ble_scan_init(void)
{
    BLEAppUtil_EventHandler_t event_cfg =
    {
        .handlerType   = BLEAPPUTIL_GAP_SCAN_TYPE,
        .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_SCAN_TYPE),
        .eventMask     = BLEAPPUTIL_SCAN_ENABLED | BLEAPPUTIL_SCAN_DISABLED
    };

    BLEAppUtil_registerEventHandler(&event_cfg);

    BLEAppUtil_scanInit(&centralScanInitParams);
    BLEAppUtil_setConnParams(&centralConnInitParams);
}

void ble_scan_start(void)
{
    BLEAppUtil_scanStart(&(BLEAppUtil_ScanStart_t){40, 0, 10});
}

void ble_scan_stop(void)
{
    BLEAppUtil_scanStop();
}
