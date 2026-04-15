#include <stdbool.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/ITM.h>
#include <ti/log/LogSinkITM.h>

static bool m_enabled;

void log_backend_itm_init(void)
{
    if (!m_enabled)
    {
        m_enabled = true;

        // LogSinkITM_init();
    }
}

// FIXME: Log_printf does not support this usage
void log_backend_itm_write(uint8_t* p_data, uint16_t data_len)
{
    uint32_t key;

    key = HwiP_disable();

    // ITM_send32Polling(LogSinkITM_STIM_HEADER, headerPtr);

    for (uint16_t i = 0; i < data_len; ++i)
    {
        ITM_send8Polling(LogSinkITM_STIM_TRACE, p_data[i]);
    }

    HwiP_restore(key);
}

void log_backend_uninit(void)
{
    uint32_t key = HwiP_disable();

    ITM_close();

    HwiP_restore(key);
}