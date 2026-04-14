#include "ti_ble_config.h"
#include "bleapputil_api.h"
#include "ble_util.h"
#include "ble_dev_info.h"
#include "util_tool.h"
#include "ble_adv.h"
#include "ble_scan.h"
#include "log.h"

#if defined CHANNEL_SOUNDING
#include "ble_cs.h"
#endif

DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_GAP_CONN_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_HCI_GAP_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_PASSCODE_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_PAIR_STATE_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_GATT_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_L2CAP_DATA_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_L2CAP_SIGNAL_TYPE);

static BLEAppUtil_EventHandler_t m_peri_event_cfg =
{
    .handlerType   = BLEAPPUTIL_GAP_CONN_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_CONN_TYPE),
    .eventMask     = BLEAPPUTIL_LINK_ESTABLISHED_EVENT |
                     BLEAPPUTIL_LINK_PARAM_UPDATE_REQ_EVENT |
                     BLEAPPUTIL_LINK_TERMINATED_EVENT,
};

static BLEAppUtil_EventHandler_t m_hci_event_cfg =
{
    .handlerType   = BLEAPPUTIL_HCI_GAP_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_HCI_GAP_TYPE),
    .eventMask     = BLEAPPUTIL_HCI_COMMAND_STATUS_EVENT_CODE |
                     BLEAPPUTIL_HCI_LE_EVENT_CODE
};

static BLEAppUtil_EventHandler_t m_pair_passcode_event_cfg =
{
    .handlerType   = BLEAPPUTIL_PASSCODE_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_PASSCODE_TYPE),
};

static BLEAppUtil_EventHandler_t m_pair_state_event_cfg =
{
    .handlerType   = BLEAPPUTIL_PAIR_STATE_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_PAIR_STATE_TYPE),
    .eventMask     = BLEAPPUTIL_PAIRING_STATE_STARTED |
                     BLEAPPUTIL_PAIRING_STATE_COMPLETE |
                     BLEAPPUTIL_PAIRING_STATE_ENCRYPTED |
                     BLEAPPUTIL_PAIRING_STATE_BOND_SAVED,
};

static BLEAppUtil_EventHandler_t m_gatt_event_cfg =
{
    .handlerType   = BLEAPPUTIL_GATT_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GATT_TYPE),
    .eventMask     = BLEAPPUTIL_ATT_FLOW_CTRL_VIOLATED_EVENT |
                     BLEAPPUTIL_ATT_MTU_UPDATED_EVENT
};

static BLEAppUtil_EventHandler_t m_l2cap_data_event_cfg =
{
    .handlerType   = BLEAPPUTIL_L2CAP_DATA_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_L2CAP_DATA_TYPE),
    .eventMask     = 0,
};

static BLEAppUtil_EventHandler_t m_l2cap_signal_event_cfg =
{
    .handlerType   = BLEAPPUTIL_L2CAP_SIGNAL_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_L2CAP_SIGNAL_TYPE),
    .eventMask     = BLEAPPUTIL_L2CAP_CHANNEL_ESTABLISHED_EVT       |
                     BLEAPPUTIL_L2CAP_CHANNEL_TERMINATED_EVT        |
                     BLEAPPUTIL_L2CAP_OUT_OF_CREDIT_EVT             |
                     BLEAPPUTIL_L2CAP_PEER_CREDIT_THRESHOLD_EVT
};

static BLEAppUtil_GeneralParams_t m_general_param =
{
    .taskPriority         = 1,
    .taskStackSize        = 1024,
    .profileRole          = 0,                                      /* Defined in syscfg file */
    .addressMode          = DEFAULT_ADDRESS_MODE,                   /* Defined in syscfg file */
    .deviceNameAtt        = attDeviceName,                          /* Defined in syscfg file */
    .pDeviceRandomAddress = pRandomAddress,                         /* Defined in syscfg file */
};

static BLEAppUtil_PeriCentParams_t m_peri_cent_param =
{
    .connParamUpdateDecision = DEFAULT_PARAM_UPDATE_REQ_DECISION,
    .gapBondParams           = &gapBondParams                       /* Defined in syscfg file */
};

static void stack_critical_error(int32 errorCode, void* pInfo)
{
    log_error("BLE stack critical error: %d", errorCode);
}

static void stack_init_done(gapDeviceInitDoneEvent_t* deviceInitDoneData)
{
    memcpy(g_ble_dev_info.mac_addr_le, deviceInitDoneData->devAddr, 6);
    memcpy(g_ble_dev_info.adv_addr_le, GAP_GetDevAddress(FALSE), 6);

    //    DevInfo_start();
    //    SimpleGatt_start();

    BLEAppUtil_registerEventHandler(&m_peri_event_cfg);
    BLEAppUtil_registerEventHandler(&m_hci_event_cfg);
    BLEAppUtil_registerEventHandler(&m_pair_passcode_event_cfg);
    BLEAppUtil_registerEventHandler(&m_pair_state_event_cfg);
    BLEAppUtil_registerEventHandler(&m_gatt_event_cfg);
    BLEAppUtil_registerEventHandler(&m_l2cap_data_event_cfg);
    BLEAppUtil_registerEventHandler(&m_l2cap_signal_event_cfg);

#if defined CHANNEL_SOUNDING
    ble_cs_init();
#endif

    ble_adv_init();
    ble_scan_init();

    ble_adv_start();
}

void ble_host_init(void)
{
    m_general_param.profileRole = profileRole;

    BLEAppUtil_init(&stack_critical_error, &stack_init_done, &m_general_param, &m_peri_cent_param);
}