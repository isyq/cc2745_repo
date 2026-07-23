#include "ranging/ranging_profile.h"
#include "ti_ble_config.h"
#include "bleapputil_api.h"
#include "ble_cs_config.h"
#include "ble_util.h"
#include "log.h"

#define RAS_SERVER_REAL_TIME_EN 0

#define KEY_NODE_INVALID_PROCEDURE_COUNTER 0xFFFFFFFF

static uint32_t m_proc_counts[MAX_NUM_BLE_CONNS];
static uint8_t m_antenna_paths[MAX_NUM_BLE_CONNS];
static uint32_t m_curr_proc_count = KEY_NODE_INVALID_PROCEDURE_COUNTER;

static uint8_t handle_procedure_enable(CS_procEnableCompleteEvt_t* p_event)
{
    if (p_event == NULL)
    {
        return INVALIDPARAMETER;
    }

    if (p_event->csStatus == SUCCESS)
    {
        if (RRSP_RegistrationStatus(p_event->connHandle) != RRSP_UNREGISTER)
        {
            uint8_t ant_path_mask = CS_calcAntPathsMask(p_event->ACI);

            if (ant_path_mask != 0)
            {
                m_antenna_paths[p_event->connHandle] = ant_path_mask;
            }
            else
            {
                return INVALIDPARAMETER;
            }
        }
    }

    return SUCCESS;
}

static uint8_t handle_subevent_result(CS_subeventResultsEvt_t* p_event)
{
    if (p_event == NULL)
    {
        return INVALIDPARAMETER;
    }

    uint8_t status = INVALIDPARAMETER;
    Ranging_RangingHeader_t ranging_header;

    RRSP_RegistrationStatus_e reg_status = RRSP_RegistrationStatus(p_event->connHandle);
    if (reg_status != RRSP_UNREGISTER)
    {
        status = SUCCESS;

        if (p_event->procedureDoneStatus == CS_PROCEDURE_ABORTED)
        {
            status = FAILURE;
        }

        if (status == SUCCESS && m_proc_counts[p_event->connHandle] != p_event->procedureCounter)
        {
            m_proc_counts[p_event->connHandle] = p_event->procedureCounter;

            ranging_header.antennaPathsMask = m_antenna_paths[p_event->connHandle];

            if (ranging_header.antennaPathsMask != 0)
            {
                ranging_header.configID        = p_event->configID;
                ranging_header.rangingCounter  = p_event->procedureCounter;
                ranging_header.selectedTxPower = p_event->referencePowerLevel;

                status = RRSP_ProcedureStarted(p_event->connHandle,
                                               (uint16_t)m_proc_counts[p_event->connHandle],
                                               (uint8_t*)&ranging_header);
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

static uint8_t handle_subevent_result_continue(CS_subeventResultsContinueEvt_t* p_event)
{
    if (p_event == NULL)
    {
        return INVALIDPARAMETER;
    }

    uint8_t status = INVALIDPARAMETER;

    if (RRSP_RegistrationStatus(p_event->connHandle) != RRSP_UNREGISTER)
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

        if ((status == SUCCESS) && (p_event->procedureDoneStatus == CS_PROCEDURE_DONE))
        {
            status = RRSP_ProcedureDone(p_event->connHandle, (uint16_t)m_proc_counts[p_event->connHandle]);
        }
    }

    return status;
}

static void HANDLER_NAME(BLEAPPUTIL_CS_TYPE)(uint32 event, BLEAppUtil_msgHdr_t* p_msg_data)
{
    if (event != BLEAPPUTIL_CS_EVENT_CODE)
    {
        return;
    }

    switch (((csEvtHdr_t*)p_msg_data)->opcode)
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

        handle_subevent_result(p_event_data);

        uint8_t proc_status = p_event_data->procedureDoneStatus;
        if (proc_status == CS_PROCEDURE_DONE || proc_status == CS_PROCEDURE_ABORTED)
        {
            m_curr_proc_count = KEY_NODE_INVALID_PROCEDURE_COUNTER;
        }
    }
    break;

    case CS_SUBEVENT_CONTINUE_RESULT:
    {
        CS_subeventResultsContinueEvt_t* p_event_data = (CS_subeventResultsContinueEvt_t*)p_msg_data;

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

                handle_subevent_result_continue(p_conti_event);

                ICall_free(p_conti_event);
            }

            ICall_free(p_conti_event);
        }

        uint8_t proc_status = p_event_data->procedureDoneStatus;
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

static void handle_cccd_update(uint16_t conn_handle, uint16_t p_value)
{
    return;
}

static void handle_status_update(uint8_t status, uint16_t conn_handle, uint16_t ranging_counter)
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

static RRSP_cb_t m_ras_server_callback = {handle_cccd_update, handle_status_update};
DEF_STATIC_BLE_EVENT_CFG(m_cs_event_cfg, BLEAPPUTIL_CS_TYPE, BLEAPPUTIL_CS_EVENT_CODE)

void ble_ras_server_init(void)
{
    memset(m_proc_counts, 0xFF, MAX_NUM_BLE_CONNS * sizeof(m_proc_counts[0]));
    memset(m_antenna_paths, 0, MAX_NUM_BLE_CONNS * sizeof(m_antenna_paths[0]));

    BLEAppUtil_registerEventHandler(&m_cs_event_cfg);

    RRSP_start(&m_ras_server_callback, RAS_SERVER_REAL_TIME_EN);
}
