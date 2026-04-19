#include "hal_os_api.h"
#include "hal_drv_api.h"
#include "log.h"

#define UART_INST 0

static uint8_t m_uart_rx_buffer[16];
static hal_mutex_t m_uart_mutex;

static void uart_rx_isr(uint8_t* p_data, uint16_t data_len)
{
    hal_mutex_unlock(&m_uart_mutex);
}

static void runner_task_entry(void* arg)
{
    hal_mutex_init(&m_uart_mutex);

    hal_uart_init(UART_INST, 115200, uart_rx_isr);
    hal_uart_send(UART_INST, (uint8_t*)"Hello\r\n", 7);

    for (;;)
    {
        hal_uart_receive(UART_INST, m_uart_rx_buffer, 3);
        
        hal_mutex_lock(&m_uart_mutex);

        m_uart_rx_buffer[3] = '\r';
        m_uart_rx_buffer[4] = '\n';
        hal_uart_send(UART_INST, m_uart_rx_buffer, 5);
    }
}

void runner_uart_task_init(void)
{
    hal_task_create(&(hal_task_param_t){
        .name = "UART",
        .stack_size = 1024,
        .priority = 5,
        .entry_fn = runner_task_entry,
    });
}

