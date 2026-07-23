#include "ti_ble_config.h"
#include "bleapputil_api.h"
#include "ble_util.h"
#include "ble_scan_param.h"
#include "log.h"

const BLEAppUtil_ConnParams_t centralConnInitParams =
{
    .initPhys        = BLE_PARAM_INITIATOR_PHY,
    .scanInterval    = BLE_PARAM_CONN_SCAN_INTERVAL,     /* Units of 0.625ms */
    .scanWindow      = BLE_PARAM_CONN_SCAN_WINDOW,       /* Units of 0.625ms */
    .minConnInterval = BLE_PARAM_CONN_INTERVAL_MIN,      /* Units of 1.25ms  */
    .maxConnInterval = BLE_PARAM_CONN_INTERVAL_MAX,      /* Units of 1.25ms  */
    .connLatency     = BLE_PARAM_SLAVE_LATENCY,
    .supTimeout      = BLE_PARAM_SUP_TIMEOUT             /* Units of 10ms */
};

const BLEAppUtil_ScanInit_t centralScanInitParams =
{
    .primPhy         = BLE_PARAM_SCAN_PRIMARY_PHY,
    .scanType        = BLE_PARAM_SCAN_TYPE,
    .scanInterval    = BLE_PARAM_SCAN_INTERVAL,
    .scanWindow      = BLE_PARAM_SCAN_WINDOW,
    .advReportFields = BLE_PARAM_ADV_REPORT_FIELDS,
    .scanPhys        = BLE_PARAM_SCAN_PHYS,
    .fltPolicy       = BLE_PARAM_FILTER_POLICY,
    .fltPduType      = BLE_PARAM_FILTER_PDU_TYPE,
    .fltMinRssi      = BLE_PARAM_FILTER_MIN_RSSI,
    .fltDiscMode     = BLE_PARAM_FILTER_DISC_MODE,
    .fltDup          = BLE_PARAM_DUPLICATE_FILTER
};

BLEAppUtil_ConnectParams_t centralConnParams =
{
    .phys    = BLE_PARAM_INITIATOR_PHY,
    .timeout = 0
};

static void HANDLER_NAME(BLEAPPUTIL_GAP_SCAN_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* pMsgData)
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

DEF_STATIC_BLE_EVENT_CFG(m_scan_event_cfg, BLEAPPUTIL_GAP_SCAN_TYPE,
                         BLEAPPUTIL_SCAN_ENABLED |
                         BLEAPPUTIL_SCAN_DISABLED);

void ble_scan_init(void)
{
    BLEAppUtil_registerEventHandler(&m_scan_event_cfg);
    BLEAppUtil_scanInit(&centralScanInitParams);
    BLEAppUtil_setConnParams(&centralConnInitParams);
}

void ble_scan_start(void)
{
    log_info("Start scan");

    BLEAppUtil_scanStart(&(BLEAppUtil_ScanStart_t){40, 0, 10});
}

void ble_scan_stop(void)
{
    BLEAppUtil_scanStop();
}
