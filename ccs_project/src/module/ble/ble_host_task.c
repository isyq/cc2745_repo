#include "ti_ble_config.h"
#include "bleapputil_api.h"
#include "log.h"
#include "ble_util.h"
#include "ble_dev_info.h"
#include "util_tool.h"
#include "ble_adv.h"
#include "ble_scan.h"

DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_GAP_CONN_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_GAP_ADV_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_GAP_SCAN_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_HCI_GAP_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_PASSCODE_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_PAIR_STATE_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_GATT_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_L2CAP_DATA_TYPE);
DEF_WEAK_BLE_EVENT_HANDLER(BLEAPPUTIL_L2CAP_SIGNAL_TYPE);

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

BLEAppUtil_EventHandler_t m_peri_event_cfg =
{
    .handlerType   = BLEAPPUTIL_GAP_CONN_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_CONN_TYPE),
    .eventMask     = BLEAPPUTIL_LINK_ESTABLISHED_EVENT |
                     BLEAPPUTIL_LINK_PARAM_UPDATE_REQ_EVENT |
                     BLEAPPUTIL_LINK_TERMINATED_EVENT,
};

BLEAppUtil_EventHandler_t m_adv_event_cfg =
{
    .handlerType   = BLEAPPUTIL_GAP_ADV_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_ADV_TYPE),
    .eventMask     = BLEAPPUTIL_ADV_START_AFTER_ENABLE |
                     BLEAPPUTIL_ADV_END_AFTER_DISABLE
};

BLEAppUtil_EventHandler_t m_scan_event_cfg =
{
    .handlerType   = BLEAPPUTIL_GAP_SCAN_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_SCAN_TYPE),
    .eventMask     = BLEAPPUTIL_SCAN_ENABLED |
                     BLEAPPUTIL_SCAN_DISABLED
};

BLEAppUtil_EventHandler_t m_hci_event_cfg =
{
    .handlerType   = BLEAPPUTIL_HCI_GAP_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_HCI_GAP_TYPE),
    .eventMask     = BLEAPPUTIL_HCI_COMMAND_STATUS_EVENT_CODE |
                     BLEAPPUTIL_HCI_LE_EVENT_CODE
};

BLEAppUtil_EventHandler_t m_pair_passcode_event_cfg =
{
    .handlerType   = BLEAPPUTIL_PASSCODE_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_PASSCODE_TYPE),
};

BLEAppUtil_EventHandler_t m_pair_state_event_cfg =
{
    .handlerType   = BLEAPPUTIL_PAIR_STATE_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_PAIR_STATE_TYPE),
    .eventMask     = BLEAPPUTIL_PAIRING_STATE_STARTED |
                     BLEAPPUTIL_PAIRING_STATE_COMPLETE |
                     BLEAPPUTIL_PAIRING_STATE_ENCRYPTED |
                     BLEAPPUTIL_PAIRING_STATE_BOND_SAVED,
};

BLEAppUtil_EventHandler_t m_gatt_event_cfg =
{
    .handlerType   = BLEAPPUTIL_GATT_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GATT_TYPE),
    .eventMask     = BLEAPPUTIL_ATT_FLOW_CTRL_VIOLATED_EVENT |
                     BLEAPPUTIL_ATT_MTU_UPDATED_EVENT
};

BLEAppUtil_EventHandler_t m_l2cap_data_event_cfg =
{
    .handlerType   = BLEAPPUTIL_L2CAP_DATA_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_L2CAP_DATA_TYPE),
    .eventMask     = 0,
};

BLEAppUtil_EventHandler_t m_l2cap_signal_event_cfg =
{
    .handlerType   = BLEAPPUTIL_L2CAP_SIGNAL_TYPE,
    .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_L2CAP_SIGNAL_TYPE),
    .eventMask     = BLEAPPUTIL_L2CAP_CHANNEL_ESTABLISHED_EVT       |
                     BLEAPPUTIL_L2CAP_CHANNEL_TERMINATED_EVT        |
                     BLEAPPUTIL_L2CAP_OUT_OF_CREDIT_EVT             |
                     BLEAPPUTIL_L2CAP_PEER_CREDIT_THRESHOLD_EVT
};

static void stack_critical_error(int32 errorCode, void* pInfo)
{
    log_error("BLE stack critical error: %d", errorCode);
}

static void stack_init_done(gapDeviceInitDoneEvent_t* deviceInitDoneData)
{
    memcpy(g_ble_dev_info.mac_addr_le, deviceInitDoneData->devAddr, 6);

    if (m_general_param.addressMode > ADDRMODE_RANDOM)
    {
        memcpy(g_ble_dev_info.adv_addr_le, GAP_GetDevAddress(FALSE), 6);
    }
//    DevInfo_start();
//    SimpleGatt_start();

    BLEAppUtil_registerEventHandler(&m_peri_event_cfg);
    BLEAppUtil_registerEventHandler(&m_adv_event_cfg);
    BLEAppUtil_registerEventHandler(&m_scan_event_cfg);
    BLEAppUtil_registerEventHandler(&m_hci_event_cfg);
    BLEAppUtil_registerEventHandler(&m_pair_passcode_event_cfg);
    BLEAppUtil_registerEventHandler(&m_pair_state_event_cfg);
    BLEAppUtil_registerEventHandler(&m_gatt_event_cfg);
    BLEAppUtil_registerEventHandler(&m_l2cap_data_event_cfg);
    BLEAppUtil_registerEventHandler(&m_l2cap_signal_event_cfg);

    ble_adv_init();
    ble_scan_init();

    ble_adv_start();
}

static void init_ble_dev_info(void)
{
    memset(&g_ble_dev_info, 0, sizeof(g_ble_dev_info));

    g_ble_dev_info.addr_type       = DEFAULT_ADDRESS_MODE;
    g_ble_dev_info.device_name     = attDeviceName;
    g_ble_dev_info.device_name_len = strlen((char*)attDeviceName);
}

void ble_host_task_init(void)
{
    init_ble_dev_info();

    m_general_param.profileRole = profileRole;

    BLEAppUtil_init(&stack_critical_error, &stack_init_done, &m_general_param, &m_peri_cent_param);
}
