#include "ti_ble_config.h"
#include "bleapputil_api.h"
#include "ble_util.h"
#include "ble_adv.h"
#include "log.h"
#include "ble_link.h"

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_CONN_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    switch (event)
    {
    case BLEAPPUTIL_LINK_ESTABLISHED_EVENT:
    {
        gapEstLinkReqEvent_t* p_event = (gapEstLinkReqEvent_t*)p_msg_data;

        log_info("Connected: %d/%d", p_event->connectionHandle, ble_link_count());

        ble_link_add(p_event->connectionHandle, p_event->connRole, p_event->devAddrType, p_event->devAddr);

        attExchangeMTUReq_t mtu_req = {MAX_PDU_SIZE - L2CAP_HDR_SIZE};
        GATT_ExchangeMTU(p_event->connectionHandle, &mtu_req, BLEAppUtil_getSelfEntity());

        if (linkDB_NumActive() < linkDB_NumConns())
        {
            ble_adv_start();
        }
        else
        {
            ble_adv_stop();
        }
    }
    break;

    case BLEAPPUTIL_LINK_TERMINATED_EVENT:
    {
        gapTerminateLinkEvent_t* p_event = (gapTerminateLinkEvent_t*)p_msg_data;

        ble_link_remove(p_event->connectionHandle);

        log_info("Disconnected: %d/%d", p_event->connectionHandle, ble_link_count());

        ble_adv_start();
    }
    break;

    case BLEAPPUTIL_LINK_PARAM_UPDATE_REQ_EVENT:
    {
        gapUpdateLinkParamReqEvent_t* pReq = (gapUpdateLinkParamReqEvent_t*)p_msg_data;

        BLEAppUtil_paramUpdateRsp(pReq, TRUE);
    }
    break;

    case BLEAPPUTIL_LINK_PARAM_UPDATE_EVENT:
    {
//        gapLinkUpdateEvent_t* pPkt = (gapLinkUpdateEvent_t*)p_msg_data;

    }
    break;

    default:
        break;
    }
}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GATT_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    gattMsgEvent_t* gattMsg = ( gattMsgEvent_t* )p_msg_data;
    switch ( gattMsg->method )
    {
    case ATT_FLOW_CTRL_VIOLATED_EVENT:
        break;

    case ATT_MTU_UPDATED_EVENT:
        break;

    default:
        break;
    }
}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_HCI_GAP_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    switch (event)
    {
    case BLEAPPUTIL_HCI_COMMAND_STATUS_EVENT_CODE:
    {
        hciEvt_CommandStatus_t* pHciMsg = (hciEvt_CommandStatus_t*)p_msg_data;
        switch ( event )
        {
        case HCI_LE_SET_PHY:
        {
            if (pHciMsg->cmdStatus == HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE)
            {
            }
        }
        break;

        default:
            break;
        }
    }

    case BLEAPPUTIL_HCI_LE_EVENT_CODE:
    {
//        hciEvt_BLEPhyUpdateComplete_t* pPUC = (hciEvt_BLEPhyUpdateComplete_t*) p_msg_data;
    }
    break;

    default:
        break;

    }
}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_PASSCODE_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    BLEAppUtil_PasscodeData_t* pData = (BLEAppUtil_PasscodeData_t*)p_msg_data;

    // Send passcode response
    GAPBondMgr_PasscodeRsp(pData->connHandle, SUCCESS, B_APP_DEFAULT_PASSCODE);
}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_PAIR_STATE_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    BLEAppUtil_PairStateData_t* p_event_data = (BLEAppUtil_PairStateData_t*)p_msg_data;
    ble_link_t* p_link                       = ble_link_get(p_event_data->connHandle);

    switch (event)
    {
    case BLEAPPUTIL_PAIRING_STATE_STARTED:
    {
    }
    break;

    case BLEAPPUTIL_PAIRING_STATE_COMPLETE:
    {
#if defined RANGING_SERVER
        if (!p_link->param.is_self_central)
        {
            CS_securityEnableCmdParams_t param = {p_event_data->connHandle};
            CS_SecurityEnable(&param);
        }
#endif
    }
    break;

    case BLEAPPUTIL_PAIRING_STATE_ENCRYPTED:
    {
#if defined RANGING_SERVER
        if (!p_link->param.is_self_central)
        {
            CS_securityEnableCmdParams_t param = {p_event_data->connHandle};
            CS_SecurityEnable(&param);
        }
#endif

#if defined RANGING_CLIENT
     ble_cs_client_enable(p_event_data->connHandle, 1); // On-demond mode
#endif
    }
    break;

    case BLEAPPUTIL_PAIRING_STATE_BOND_SAVED:
    {
#if defined RANGING_CLIENT
     ble_cs_client_enable(p_event_data->connHandle, 1);
#endif
    }
    break;

    default:
        break;
    }

}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_L2CAP_DATA_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{

}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_L2CAP_SIGNAL_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{

}
