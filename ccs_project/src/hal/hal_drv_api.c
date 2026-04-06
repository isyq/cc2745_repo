#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>
#include "ti_drivers_config.h"
#include "hal_drv_api.h"

/* ------------------------- GPIO ------------------------- */

static uint32_t m_gpio_in_pull_opt[]  = {GPIO_CFG_IN_NOPULL, GPIO_CFG_IN_PU, GPIO_CFG_IN_PD};
static uint32_t m_gpio_int_type_opt[] = {GPIO_CFG_IN_INT_NONE, GPIO_CFG_IN_INT_FALLING, GPIO_CFG_IN_INT_RISING, GPIO_CFG_IN_INT_BOTH_EDGES};
static uint32_t m_gpio_out_pull_opt[] = {GPIO_CFG_OUT_STD, GPIO_CFG_OUT_OD_NOPULL, GPIO_CFG_OUT_OD_PU, GPIO_CFG_OUT_OD_PD};

void hal_gpio_init(void)
{
    static bool is_gpio_init = false;

    if (!is_gpio_init)
    {
        GPIO_init();

        is_gpio_init = true;
    }
}

void hal_gpio_create_in_pin(uint8_t pin, uint8_t pull, uint8_t int_type, hal_gpio_isr_fn isr_fn)
{
    hal_gpio_init();

    GPIO_PinConfig config = m_gpio_in_pull_opt[pull] | m_gpio_int_type_opt[int_type];

    GPIO_setConfig(pin, config);
    GPIO_setCallback(pin, isr_fn);
}

void hal_gpio_create_out_pin(uint8_t pin, uint8_t pull, uint8_t level)
{
    hal_gpio_init();

    GPIO_PinConfig config = m_gpio_out_pull_opt[pull] | (level ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);

    GPIO_setConfig(pin, config);
}

void hal_gpio_enable_interrupt(uint8_t pin)
{
    GPIO_enableInt(pin);
}

uint8_t hal_gpio_read_pin(uint8_t pin)
{
    return GPIO_read(pin);
}

void hal_gpio_write_pin(uint8_t pin, uint8_t level)
{
    GPIO_write(pin, level);
}

void hal_gpio_toggle_pin(uint8_t pin)
{
    GPIO_toggle(pin);
}

/* ------------------------- UART ------------------------- */

static UART2_Handle m_uart_handles[2];
static hal_uart_isr_fn m_uart_isrs[2];

static void uart_read_data_isr_0(UART2_Handle handle, void* p_buffer, size_t len, void* user_arg, int_fast16_t status)
{
    m_uart_isrs[0](p_buffer, len);
}

static void uart_read_data_isr_1(UART2_Handle handle, void* p_buffer, size_t len, void* user_arg, int_fast16_t status)
{
    m_uart_isrs[1](p_buffer, len);
}

void hal_uart_init(uint8_t inst, uint32_t baud_rate, hal_uart_isr_fn isr_fn)
{
    UART2_Params param;

    UART2_Params_init(&param);

    param.baudRate = baud_rate;

    if (isr_fn != NULL)
    {
        param.readMode     = UART2_Mode_CALLBACK;
        param.readCallback = inst == 0 ? uart_read_data_isr_0 : uart_read_data_isr_1;

        m_uart_isrs[inst] = isr_fn;
    }

    m_uart_handles[inst] = UART2_open(inst, &param);
}

uint16_t hal_uart_send(uint8_t inst, uint8_t* p_data, uint16_t data_len)
{
    return UART2_write(m_uart_handles[inst], p_data, data_len, NULL);
}

uint16_t hal_uart_receive(uint8_t inst, uint8_t* p_data, uint16_t data_len)
{
    return UART2_read(m_uart_handles[inst], p_data, data_len, NULL);
}
