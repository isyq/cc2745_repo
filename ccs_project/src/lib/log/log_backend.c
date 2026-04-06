#include "log_backend.h"
#include "log_backend_uart.h"
#include "log_backend_itm.h"

static uint8_t m_log_backend;
static log_backend_print_fn m_print_impl;

void log_backend_init(uint8_t backend)
{
    m_log_backend = backend;

    switch (backend)
    {
    case LOG_BACKEND_UART:
        log_backend_uart_init();

        m_print_impl = log_backend_uart_write;
        break;

    case LOG_BACKEND_ITM:
        log_backend_itm_init();

        m_print_impl = log_backend_itm_write;
        break;

    default:
        break;
    }
}

void log_backend_print(log_level_t level, void* p_data, uint16_t data_len)
{
    m_print_impl((uint8_t*)p_data, data_len);
}
