#include "bleapputil_api.h"
#include "ble_util.h"
#include "ble_adv.h"
#include "log.h"
#include "ble_link.h"

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GAP_CONN_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* pMsgData)
{
    switch (event)
    {
    case BLEAPPUTIL_LINK_ESTABLISHED_EVENT:
    {
        gapEstLinkReqEvent_t* p_event = (gapEstLinkReqEvent_t*)pMsgData;

        log_info("Connected: %d/%d", p_event->connectionHandle, ble_link_count());

        ble_link_add(p_event->connectionHandle, p_event->connRole, p_event->devAddrType, p_event->devAddr);

        if(linkDB_NumActive() < linkDB_NumConns())
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
        gapTerminateLinkEvent_t* p_event = (gapTerminateLinkEvent_t*)pMsgData;

        ble_link_remove(p_event->connectionHandle);

        log_info("Disconnected: %d/%d", p_event->connectionHandle, ble_link_count());

        ble_adv_start();
    }
    break;

    case BLEAPPUTIL_LINK_PARAM_UPDATE_REQ_EVENT:
    {
        gapUpdateLinkParamReqEvent_t* pReq = (gapUpdateLinkParamReqEvent_t*)pMsgData;

        BLEAppUtil_paramUpdateRsp(pReq, TRUE);
    }
    break;

    case BLEAPPUTIL_LINK_PARAM_UPDATE_EVENT:
    {
        gapLinkUpdateEvent_t* pPkt = (gapLinkUpdateEvent_t*)pMsgData;

    }
    break;

    default:
        break;
    }
}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_GATT_TYPE)(uint32 event, BLEAppUtil_msgHdr_t*pMsgData)
{
    gattMsgEvent_t* gattMsg = ( gattMsgEvent_t* )pMsgData;
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

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_HCI_GAP_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* pMsgData)
{
    switch (event)
    {
    case BLEAPPUTIL_HCI_COMMAND_STATUS_EVENT_CODE:
    {
        hciEvt_CommandStatus_t* pHciMsg = (hciEvt_CommandStatus_t*)pMsgData;
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
        hciEvt_BLEPhyUpdateComplete_t* pPUC = (hciEvt_BLEPhyUpdateComplete_t*) pMsgData;
    }
    break;

    default:
        break;

    }
}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_PASSCODE_TYPE)(uint32 event, BLEAppUtil_msgHdr_t*pMsgData)
{
    BLEAppUtil_PasscodeData_t* pData = (BLEAppUtil_PasscodeData_t*)pMsgData;

    // Send passcode response
    GAPBondMgr_PasscodeRsp(pData->connHandle, SUCCESS, B_APP_DEFAULT_PASSCODE);
}

void Pairing_pairStateHandler(uint32 event, BLEAppUtil_msgHdr_t* pMsgData)
{
    switch (event)
    {
    case BLEAPPUTIL_PAIRING_STATE_STARTED:
    {
    }
    break;
    
    case BLEAPPUTIL_PAIRING_STATE_COMPLETE:
    {
    }
    break;

    case BLEAPPUTIL_PAIRING_STATE_ENCRYPTED:
    {
    }
    break;

    case BLEAPPUTIL_PAIRING_STATE_BOND_SAVED:
    {
    }
    break;

    default:
    break;
    }

}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_L2CAP_DATA_TYPE)(uint32 event, BLEAppUtil_msgHdr_t *pMsgData)
{

}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_L2CAP_SIGNAL_TYPE)(uint32 event, BLEAppUtil_msgHdr_t *pMsgData)
{

}
