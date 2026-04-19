#include <stdbool.h>
#include <ti/drivers/ITM.h>

#define LOG_ITM_PORT 0

static bool m_enabled = false;

void log_backend_itm_init(void)
{
    if (!m_enabled)
    {
        m_enabled = true;

        ITM_open();
    }
}

void log_backend_itm_write(uint8_t* p_data, uint16_t data_len)
{
    ITM_sendBufferAtomic(LOG_ITM_PORT, (char*)p_data, data_len);
}

void log_backend_uninit(void)
{
    ITM_close();
}