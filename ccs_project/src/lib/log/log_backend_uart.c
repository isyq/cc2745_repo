#include <stdbool.h>
#include <string.h>
#include "hal_drv_api.h"

#ifndef CFG_LOG_UART_BAUD_RATE
#define CFG_LOG_UART_BAUD_RATE 115200
#endif

#ifndef CFG_LOG_UART_INSTANCE
#define CFG_LOG_UART_INSTANCE 0
#endif

static bool m_enabled;

void log_backend_uart_init(void)
{
    if (!m_enabled)
    {
        hal_uart_init(CFG_LOG_UART_INSTANCE, CFG_LOG_UART_BAUD_RATE, NULL);

        m_enabled = true;
    }
}

void log_backend_uart_write(uint8_t* p_data, uint16_t data_len)
{
    hal_uart_send(CFG_LOG_UART_INSTANCE, p_data, data_len);
}
