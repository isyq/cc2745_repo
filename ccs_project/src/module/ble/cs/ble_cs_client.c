#include "ranging/ranging_profile_client.h"
#include "ranging/ranging_profile.h"
#include "bleapputil_timers.h"
#include "bleapputil_api.h"
#include "ble_util.h"

// Timeout for waiting for control point response in milliseconds (On-Demand mode only)
#define RREQ_TIMEOUT_CONTROL_POINT_RSP_MS RREQ_MAX_TIMEOUT_CONTROL_POINT_RSP_MS
// Timeout for data ready event in milliseconds (On-Demand mode only)
#define RREQ_TIMEOUT_DATA_READY_MS RREQ_MAX_TIMEOUT_DATA_READY_MS
// Timeout for first segment event in milliseconds (On-Demand & Real-Time)
#define RREQ_TIMEOUT_FIRST_SEGMENT_MS RREQ_MAX_TIMEOUT_FIRST_SEGMENT_MS
// Timeout for next segment event in milliseconds (On-Demand & Real-Time)
#define RREQ_TIMEOUT_NEXT_SEGMENT_MS RREQ_MAX_TIMEOUT_NEXT_SEGMENT_MS

typedef struct
{
    uint16_t connHandle;
    RREQEnableModeType_e enableMode;
}AppRREQ_enableData_t;

BLEAppUtil_timerHandle gEnableTimerHandle = BLEAPPUTIL_TIMER_INVALID_HANDLE;

CarNode_session gSessionsDb[CAR_NODE_MAX_CONNS];

void CarNode_handleReadRemoteCapsComplete(ChannelSounding_readRemoteCapabEvent_t* pCsReadRemoteCapsEvt)
{
#ifdef CS_MEASURE_DISTANCE
    if (pCsReadRemoteCapsEvt != NULL &&
        pCsReadRemoteCapsEvt->connHandle < CAR_NODE_MAX_CONNS &&
        pCsReadRemoteCapsEvt->csStatus == CS_STATUS_SUCCESS)
    {
        // Add timing parameters from the CS configuration into the relevant session
        gSessionsDb[pCsReadRemoteCapsEvt->connHandle].remoteTsW = pCsReadRemoteCapsEvt->tSwCap;
    }
#endif
}

static void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_CS_TYPE)(uint32_t event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    csEvtHdr_t* pCsEvt = (csEvtHdr_t*)p_msg_data;

    if (event == BLEAPPUTIL_CS_EVENT_CODE)
    {
        uint8_t opcode           = pCsEvt->opcode;
        uint8_t sendToExtHandler = TRUE;

        switch ( opcode )
        {
        case CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE_EVENT:
        {
            CarNode_handleReadRemoteCapsComplete((ChannelSounding_readRemoteCapabEvent_t*) pCsEvt);
            break;
        }

        case CS_CONFIG_COMPLETE_EVENT:
        {
            break;
        }

        case CS_READ_REMOTE_FAE_TABLE_COMPLETE_EVENT:
        {
            break;
        }

        case CS_SECURITY_ENABLE_COMPLETE_EVENT:
        {
            break;
        }

        case CS_PROCEDURE_ENABLE_COMPLETE_EVENT:
        {
            // Call Procedure Enable Complete handler
            CarNode_handleCsProcEnableComplete((ChannelSounding_procEnableComplete_t*) pCsEvt);
            break;
        }

        case CS_SUBEVENT_RESULT:
        {
            ChannelSounding_subeventResults_t* subeventResultsEvt = (ChannelSounding_subeventResults_t*) pCsEvt;

            sendToExtHandler = CarNode_handleCsSubeventResultsEvt(subeventResultsEvt->connHandle, CS_RESULTS_MODE_LOCAL, subeventResultsEvt);
            break;
        }

        case CS_SUBEVENT_CONTINUE_RESULT:
        {
            ChannelSounding_subeventResultsContinue_t* subeventResultsContEvt = (ChannelSounding_subeventResultsContinue_t*) pCsEvt;

            sendToExtHandler = CarNode_handleCsSubeventResultsEvtContEvt(subeventResultsContEvt->connHandle, CS_RESULTS_MODE_LOCAL,
                                                                         0xFFFF, // Procedure Counter is not used in local results
                                                                         subeventResultsContEvt);
            break;
        }

        default:
        {
            break;
        }
        }

        if (sendToExtHandler == TRUE)
        {
            // Send event to the upper layer
            CarNode_extEvtHandler(BLEAPPUTIL_CS_TYPE, BLEAPPUTIL_CS_EVENT_CODE, (BLEAppUtil_msgHdr_t*) pCsEvt);
        }
    }
}

static void client_handle_complete_event(uint16_t connHandle, uint16_t rangingCount, uint8_t status, RangingDBClient_procedureSegmentsReader_t segmentsReader)
{
    RangingDBClient_freeSegmentsReader(&segmentsReader);

    // if (gDataCompleteEvtHandler != NULL)
    // {
    //     gDataCompleteEvtHandler(connHandle, rangingCount, status, segmentsReader);
    // }
}

void ble_cs_client_init(void)
{
    BLEAppUtil_EventHandler_t m_cs_event_cfg =
    {
        .handlerType   = BLEAPPUTIL_CS_TYPE,
        .pEventHandler = DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_CS_TYPE),
        .eventMask     = BLEAPPUTIL_CS_EVENT_CODE
    };
    BLEAppUtil_registerEventHandler(&m_cs_event_cfg);

    RREQCallbacks_t client_callback =
    {
        .pDataReadyCallback         = RREQ_GetRangingData,
        .pDataCompleteEventCallback = client_handle_complete_event,
        .pStatusCallback            = NULL,
    };

    RREQConfig_t rreq_config =
    {
        .onDemandSubConfig.onDemandSubType     = RREQ_PREFER_NOTIFY,
        .onDemandSubConfig.controlPointSubType = RREQ_PREFER_NOTIFY,
        .onDemandSubConfig.dataReadySubType    = RREQ_PREFER_NOTIFY,
        .onDemandSubConfig.overwrittenSubType  = RREQ_PREFER_NOTIFY,
        .realTimeSubConfig.realTimeSubType     = RREQ_PREFER_NOTIFY,
        .timeoutConfig.timeOutControlPointRsp  = RREQ_TIMEOUT_CONTROL_POINT_RSP_MS,
        .timeoutConfig.timeOutDataReady        = RREQ_TIMEOUT_DATA_READY_MS,
        .timeoutConfig.timeOutFirstSegment     = RREQ_TIMEOUT_FIRST_SEGMENT_MS,
        .timeoutConfig.timeOutNextSegment      = RREQ_TIMEOUT_NEXT_SEGMENT_MS,
    };

    RREQ_Start(&client_callback, &rreq_config);
}

static void handle_timer_timeout(BLEAppUtil_timerHandle timerHandle, BLEAppUtil_timerTermReason_e reason, void* pData)
{
    if (reason == BLEAPPUTIL_TIMER_TIMEOUT)
    {
        if (pData != NULL)
        {
            AppRREQ_enableData_t enableData;
            enableData.connHandle = ((AppRREQ_enableData_t*)pData)->connHandle;
            enableData.enableMode = ((AppRREQ_enableData_t*)pData)->enableMode;

            BLEAppUtil_free(pData);

            gEnableTimerHandle = BLEAPPUTIL_TIMER_INVALID_HANDLE;

            AppRREQ_enable(enableData.connHandle, enableData.enableMode);
        }
    }
    else
    {
        gEnableTimerHandle = BLEAPPUTIL_TIMER_INVALID_HANDLE;
    }
}

uint8_t ble_cs_client_enable(uint16_t connHandle, uint8_t enableMode)
{
    uint8_t status = SUCCESS;

    status = RREQ_Enable(connHandle, enableMode);
    if (status == blePending)
    {
        linkDBInfo_t connInfo = {0};

        status = linkDB_GetInfo(connHandle, &connInfo);

        if ( (status != bleTimeout) && (status != bleNotConnected))
        {
            AppRREQ_enableData_t* pEnableData = BLEAppUtil_malloc(sizeof(AppRREQ_enableData_t));
            if (pEnableData != NULL)
            {
                pEnableData->connHandle = connHandle;
                pEnableData->enableMode = enableMode;

                if (gEnableTimerHandle == BLEAPPUTIL_TIMER_INVALID_HANDLE)
                {
                    gEnableTimerHandle = BLEAppUtil_startTimer(handle_timer_timeout, connInfo.connInterval, false, pEnableData);
                }

                status = blePending;
            }
            else
            {
                status = bleMemAllocError;
            }
        }
    }

    return status;
}
