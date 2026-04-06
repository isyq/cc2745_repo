#include <stdbool.h>
#include <ti\log\Log.h>

static bool m_enabled;

void log_backend_itm_init(void)
{
    if (!m_enabled)
    {
        m_enabled = true;
    }
}

// FIXME: Log_printf does not support this usage
void log_backend_itm_write(uint8_t* p_data, uint16_t data_len)
{
    p_data[data_len] = '\0';

    Log_printf(LogModule0, Log_INFO, p_data);
}
