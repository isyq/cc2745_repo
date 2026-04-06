#include <stdint.h>
#include <pmctl.h>

/**
 * Build date and time by prebuild commands:
 * 
 *    echo "%TIME%" > ${workspace_loc:/${ProjName}/src/config}/build_time.h
 *    echo "%DATE%" > ${workspace_loc:/${ProjName}/src/config}/build_date.h
 */
static const char m_build_date[] =
{
    #include "build_date.h"
};

static const char m_build_time[] =
{
    #include "build_time.h"
};

const char* fwk_get_build_date(void)
{
    return m_build_date;
}

const char* fwk_get_build_time(void)
{
    return m_build_time;
}

/**
 * Available reasons:
 *      PMCTL_RESET_SHUTDOWN_IO
 *      PMCTL_RESET_SHUTDOWN_SWD
 *      PMCTL_RESET_WATCHDOG
 *      PMCTL_RESET_SYSTEM
 *      PMCTL_RESET_CPU
 *      PMCTL_RESET_LOCKUP
 *      PMCTL_RESET_ANALOG_FSM_TIMEOUT
 *      PMCTL_RESET_EM_SENSOR
 *      PMCTL_RESET_TAMPER
 *      PMCTL_RESET_SRAM_PARITY_ERROR
 *      PMCTL_RESET_ANALOG_ERROR
 *      PMCTL_RESET_DIGITAL_ERROR
 *      PMCTL_RESET_SWD
 *      PMCTL_RESET_LFXT
 *      PMCTL_RESET_TSD
 *      PMCTL_RESET_VDDR
 *      PMCTL_RESET_VDDS
 *      PMCTL_RESET_PIN
 *      PMCTL_RESET_POR
 */
uint32_t fwk_get_reset_reason(void)
{
    return PMCTLGetResetReason();
}

void fwk_reset_system(void)
{
    PMCTLResetSystem();
}
