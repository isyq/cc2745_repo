#include "ranging/ranging_profile.h"
#include "ble_util.h"
#include "bleapputil_api.h"
#include "cs.h"

#ifndef MAX_NUM_BLE_CONNS
#define MAX_NUM_BLE_CONNS 4
#endif

#define KEY_NODE_INVALID_PROCEDURE_COUNTER 0xFFFFFFFF

static uint32_t m_proc_counts[MAX_NUM_BLE_CONNS];
static uint8_t m_antenna_paths[MAX_NUM_BLE_CONNS];
static uint32_t m_curr_proc_count = KEY_NODE_INVALID_PROCEDURE_COUNTER;

static uint8_t handle_procedure_enable(CS_procEnableCompleteEvt_t* p_event)
{
    if (p_event->csStatus == SUCCESS)
    {
        if (RRSP_RegistrationStatus(p_event->connHandle) != RRSP_UNREGISTER)
        {
            uint8_t antennaPathsMask = CS_calcAntPathsMask(p_event->ACI);

            if (antennaPathsMask != 0)
            {
                m_antenna_paths[p_event->connHandle] = antennaPathsMask;
            }
        }
    }

    return SUCCESS;
}

static uint8_t handle_sub_event(CS_subeventResultsEvt_t* p_event)
{
    uint8_t status = INVALIDPARAMETER;
    Ranging_RangingHeader_t rangingHeader;

    if ( (p_event != NULL) && (RRSP_RegistrationStatus(p_event->connHandle) != RRSP_UNREGISTER) )
    {
        status = SUCCESS;

        if (p_event->procedureDoneStatus == CS_PROCEDURE_ABORTED)
        {
            status = FAILURE;
        }

        if (status == SUCCESS && m_proc_counts[p_event->connHandle] != p_event->procedureCounter)
        {
            m_proc_counts[p_event->connHandle] = p_event->procedureCounter;

            rangingHeader.antennaPathsMask = m_antenna_paths[p_event->connHandle];

            if (rangingHeader.antennaPathsMask != 0)
            {
                rangingHeader.configID        = p_event->configID;
                rangingHeader.rangingCounter  = p_event->procedureCounter;
                rangingHeader.selectedTxPower = p_event->referencePowerLevel;

                status = RRSP_ProcedureStarted(p_event->connHandle, (uint16_t)m_proc_counts[p_event->connHandle], (uint8_t*)&rangingHeader);
            }
            else
            {
                status = FAILURE;
            }
        }

        if (status == SUCCESS)
        {
            status = RRSP_AddSubeventResult((RRSP_csSubEventResults_t*)p_event);

            if ((p_event->procedureDoneStatus == CS_PROCEDURE_DONE) && (status == SUCCESS))
            {
                status = RRSP_ProcedureDone(p_event->connHandle, (uint16_t) m_proc_counts[p_event->connHandle]);
            }
        }
    }

    return status;
}

static uint8_t handle_sub_continue_event(CS_subeventResultsContinueEvt_t* p_event)
{
    uint8_t status = INVALIDPARAMETER;

    if (p_event != NULL && RRSP_RegistrationStatus(p_event->connHandle) != RRSP_UNREGISTER)
    {
        status = SUCCESS;

        if (p_event->procedureDoneStatus == CS_PROCEDURE_ABORTED)
        {
            status = FAILURE;
        }

        if (status == SUCCESS)
        {
            status = RRSP_AddSubeventContinueResult((RRSP_csSubEventResultsContinue_t*)p_event);
        }

        if ( (status == SUCCESS) &&
             (p_event->procedureDoneStatus == CS_PROCEDURE_DONE))
        {

            status = RRSP_ProcedureDone(p_event->connHandle, (uint16_t) m_proc_counts[p_event->connHandle]);
        }
    }

    return status;
}

void DEF_BLE_EVENT_HANDLER_NAME(BLEAPPUTIL_CS_TYPE)(uint32_t event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    if (event == BLEAPPUTIL_CS_EVENT_CODE)
    {
        csEvtHdr_t* p_event_data = (csEvtHdr_t*)p_msg_data;

        switch (p_event_data->opcode)
        {
        case CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE_EVENT:
        case CS_CONFIG_COMPLETE_EVENT:
        case CS_READ_REMOTE_FAE_TABLE_COMPLETE_EVENT:
            break;

        case CS_SECURITY_ENABLE_COMPLETE_EVENT:
        {
            CS_setDefaultSettingsCmdParams_t param =
            {
                .connHandle             = 0,
                .roleEnable             = 3,
                .csSyncAntennaSelection = 1,
                .maxTxPower             = 10
            };
            CS_SetDefaultSettings(&param);
        }
        break;

        case CS_PROCEDURE_ENABLE_COMPLETE_EVENT:
        {
            CS_procEnableCompleteEvt_t* p_event_data = (CS_procEnableCompleteEvt_t*)p_msg_data;
            handle_procedure_enable(p_event_data);
        }
        break;

        case CS_SUBEVENT_RESULT:
        {
            CS_subeventResultsEvt_t* p_event_data = (CS_subeventResultsEvt_t*)p_msg_data;

            m_curr_proc_count = p_event_data->procedureCounter;
            uint8_t proc_status = p_event_data->procedureDoneStatus;

            handle_sub_event(p_event_data);

            if (proc_status == CS_PROCEDURE_DONE || proc_status == CS_PROCEDURE_ABORTED)
            {
                m_curr_proc_count = KEY_NODE_INVALID_PROCEDURE_COUNTER;
            }
        }
        break;

        case CS_SUBEVENT_CONTINUE_RESULT:
        {
            CS_subeventResultsContinueEvt_t* p_event_data = (CS_subeventResultsContinueEvt_t*)p_msg_data;

            uint8_t proc_status = p_event_data->procedureDoneStatus;
            uint16_t event_size = sizeof(CS_subeventResultsContinueEvt_t) + p_event_data->dataLen;

            if (m_curr_proc_count != KEY_NODE_INVALID_PROCEDURE_COUNTER)
            {
                CS_subeventResultsContinueEvt_t* p_conti_event = (CS_subeventResultsContinueEvt_t*)ICall_malloc(event_size);

                if (p_conti_event != NULL)
                {
                    p_conti_event->csEvtOpcode         = p_event_data->csEvtOpcode;
                    p_conti_event->connHandle          = p_event_data->connHandle;
                    p_conti_event->configID            = p_event_data->configID;
                    p_conti_event->procedureDoneStatus = p_event_data->procedureDoneStatus;
                    p_conti_event->subeventDoneStatus  = p_event_data->subeventDoneStatus;
                    p_conti_event->abortReason         = p_event_data->abortReason;
                    p_conti_event->numAntennaPath      = p_event_data->numAntennaPath;
                    p_conti_event->numStepsReported    = p_event_data->numStepsReported;
                    p_conti_event->dataLen             = p_event_data->dataLen;

                    memcpy(p_conti_event->data, p_event_data->data, p_event_data->dataLen);

                    handle_sub_continue_event(p_conti_event);

                    ICall_free(p_conti_event);
                }

                ICall_free(p_conti_event);
            }

            if (proc_status == CS_PROCEDURE_DONE || proc_status == CS_PROCEDURE_ABORTED)
            {
                m_curr_proc_count = KEY_NODE_INVALID_PROCEDURE_COUNTER;
            }
        }
        break;

        default:
            break;
        }
    }
}
