#include "ranging/ranging_profile_client.h"
#include "ranging/ranging_profile.h"
#include "bleapputil_timers.h"
#include "bleapputil_api.h"
#include "ble_util.h"
#include "ble_cs_config.h"

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


static void HANDLER_NAME(BLEAPPUTIL_CS_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    if (event != BLEAPPUTIL_CS_EVENT_CODE)
    {
        return;
    }

    csEvtHdr_t* pCsEvt = (csEvtHdr_t*)p_msg_data;

    switch (pCsEvt->opcode)
    {
    case CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE_EVENT:
    {
        CS_readRemoteCapabEvt_t* pCsReadRemoteCapsEvt = (CS_readRemoteCapabEvt_t*)pCsEvt;
        if (pCsReadRemoteCapsEvt != NULL)
        {
            if (pCsReadRemoteCapsEvt->connHandle < MAX_NUM_BLE_CONNS && pCsReadRemoteCapsEvt->csStatus == CS_STATUS_SUCCESS)
            {
                // Add timing parameters from the CS configuration into the relevant session
                gSessionsDb[pCsReadRemoteCapsEvt->connHandle].remoteTsW = pCsReadRemoteCapsEvt->tSwCap;
            }
        }

        break;
    }

    case CS_PROCEDURE_ENABLE_COMPLETE_EVENT:
    {
      CarNode_handleCsProcEnableComplete((ChannelSounding_procEnableComplete_t *) pCsEvt);
      break;
    }

    case CS_SUBEVENT_RESULT:
    {
        CS_subeventResultsEvt_t* subeventResultsEvt = (CS_subeventResultsEvt_t*) pCsEvt;
        RREQ_ProcedureStarted(subeventResultsEvt->connHandle, subeventResultsEvt->procedureCounter);
        break;
    }

    default:
    {
        break;
    }
    }
}

static void handle_data_ready(uint16_t connHandle, uint16_t rangingCount)
{
    RREQ_GetRangingData(connHandle, rangingCount);
}

static void handle_data_complete(uint16_t connHandle, uint16_t rangingCount, uint8_t status, RangingDBClient_procedureSegmentsReader_t segmentsReader)
{
    RangingDBClient_freeSegmentsReader(&segmentsReader);

    // rreq_complete_event_handler(connHandle, rangingCount, status);
}

static void handle_status(uint16_t connHandle, RREQClientStatus_e statusCode, uint8_t statusDataLen, uint8_t* statusData)
{
    switch (statusCode)
    {
    case RREQ_INIT_DONE:
    default:
        break;
    }
}

DEF_STATIC_BLE_EVENT_CFG(m_cs_event_cfg, BLEAPPUTIL_CS_TYPE, BLEAPPUTIL_CS_EVENT_CODE)

static RREQCallbacks_t m_client_callback =
{
    .pDataReadyCallback         = handle_data_ready,
    .pDataCompleteEventCallback = handle_data_complete,
    .pStatusCallback            = handle_status,
};

static RREQConfig_t m_rreq_config =
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


static void HANDLER_NAME(BLEAPPUTIL_CS_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    if (event != BLEAPPUTIL_CS_EVENT_CODE)
    {
        return;
    }
}

void ble_cs_client_init(void)
{
    BLEAppUtil_registerEventHandler(&csCmdCompleteEvtHandler);
    BLEAppUtil_registerEventHandler(&m_cs_event_cfg);

    RREQ_Start(&m_client_callback, &m_rreq_config);
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
    uint8_t status = RREQ_Enable(connHandle, enableMode);
    if (status == blePending)
    {
        linkDBInfo_t connInfo = {0};

        status = linkDB_GetInfo(connHandle, &connInfo);

        if ((status != bleTimeout) && (status != bleNotConnected))
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

uint8_t ble_cs_client_disable(uint16_t connHandle)
{
    return RREQ_Disable(connHandle);
}

uint8_t ble_cs_client_abort(uint16_t connHandle)
{
    return RREQ_Abort(connHandle);
}

uint8_t ble_cs_client_get_ranging_data(uint16_t connHandle, uint16_t rangingCount)
{
    return RREQ_GetRangingData(connHandle, rangingCount);
}

uint8_t ble_cs_client_config_char_registration(uint16_t connHandle, uint16_t charUUID, uint8_t subscribeMode)
{
    return RREQ_ConfigureCharRegistration(connHandle, charUUID, (RREQConfigSubType_e) subscribeMode);
}

uint8_t ble_cs_client_procedure_started(uint16_t connHandle, uint16_t procedureCounter)
{
    return RREQ_ProcedureStarted(connHandle, procedureCounter);
}

void rreq_complete_event_handler(uint16_t connHandle, uint16_t rangingCount, uint8_t rangingStatus, RangingDBClient_procedureSegmentsReader_t segmentsReader)
{
    bStatus_t status = (bStatus_t) rangingStatus;

    // If the connection handle is invalid, free the segments if needed and return
    if (connHandle > MAX_NUM_BLE_CONNS)
    {
        if (status == SUCCESS)
        {
            RangingDBClient_freeSegmentsReader(&segmentsReader);
        }

        return;
    }

#ifdef CS_MEASURE_DISTANCE
    CarNode_procedureInfoElem_t* pProcedureElem = NULL;

    // If the status is not SUCCESS, find a procedure element for this procedure. if found - remove it.
    // No need to free the segment reader in this case
    if (status != SUCCESS)
    {
        pProcedureElem = CarNode_getProcedureElem(connHandle, rangingCount);

        if (pProcedureElem != NULL)
        {
            CarNode_procElemRemove(connHandle, pProcedureElem);
        }
    }

    if (status == SUCCESS)
    {
        // Search for the procedure associated with the procedure counter in the list
        pProcedureElem = CarNode_getProcedureElem(connHandle, rangingCount);

        // If not found, consider it a failure
        if (pProcedureElem == NULL)
        {
            status = FAILURE;
        }

        if (status == SUCCESS)
        {
            // Add the results to the procedure element
            status = CarNode_procElemAddResultsRAS(pProcedureElem, segmentsReader);

            // Post process the addition of results.
            // Note that if the results failed to be added, this function won't free the segments reader
            CarNode_procElemAddResultsPostProcess(status, connHandle, CS_RESULTS_MODE_RAS, pProcedureElem);
        }

        if (status != SUCCESS)
        {
            // Failed to add the results - free the segments reader
            RangingDBClient_freeSegmentsReader(&segmentsReader);
        }
    }

#else
    // If we are not measuring distance, free the segments reader directly
    if (status == SUCCESS)
    {
        RangingDBClient_freeSegmentsReader(&segmentsReader);
    }
#endif // !CS_MEASURE_DISTANCE
}
