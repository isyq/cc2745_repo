#include <assert.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CLPF3.h>
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

/* ------------------------- IIC ------------------------- */

static I2C_Handle m_iic_handles[2];
static hal_iic_isr_fn m_iic_isrs[2];
static bool m_iic_enabled[2];

void on_iic_isr_0(I2C_Handle handle, I2C_Transaction* transaction, bool transferStatus)
{
    m_iic_isrs[0](transaction->readBuf, transaction->readCount);
}

void on_iic_isr_1(I2C_Handle handle, I2C_Transaction* transaction, bool transferStatus)
{
    m_iic_isrs[1](transaction->readBuf, transaction->readCount);
}

void hal_iic_init(uint8_t inst, uint8_t bit_rate, hal_iic_isr_fn isr_fn)
{
    if (m_iic_enabled[inst])
    {
        return;
    }

    assert(bit_rate <= 3);

    I2C_Params param;

    m_iic_enabled[inst] = true;

    I2C_Params_init(&param);
    param.bitRate = (I2C_BitRate)bit_rate;

    if (isr_fn != NULL)
    {
        param.transferMode        = I2C_MODE_CALLBACK;
        param.transferCallbackFxn = inst == 0 ? on_iic_isr_0 : on_iic_isr_1;

        m_iic_isrs[inst] = isr_fn;
    }

    m_iic_handles[inst] = I2C_open(inst, &param);
}

void hal_iic_send(uint8_t inst, uint16_t slave_addr, uint8_t* p_data, uint16_t data_len)
{
    if (!m_iic_enabled[inst])
    {
        return;
    }

    I2C_Transaction transaction;

    transaction.targetAddress = slave_addr;
    transaction.writeBuf      = p_data;
    transaction.writeCount    = data_len;
    transaction.readBuf       = NULL;
    transaction.readCount     = 0;

    I2C_transfer(m_iic_handles[inst], &transaction);
}

void hal_iic_send_timeout(uint8_t inst, uint16_t slave_addr, uint8_t* p_data, uint16_t data_len, uint32_t timeout)
{
    if (!m_iic_enabled[inst])
    {
        return;
    }

    I2C_Transaction transaction;

    transaction.targetAddress = slave_addr;
    transaction.writeBuf      = p_data;
    transaction.writeCount    = data_len;
    transaction.readBuf       = NULL;
    transaction.readCount     = 0;

    I2C_transferTimeout(m_iic_handles[inst], &transaction, timeout);
}

void hal_iic_receive(uint8_t inst, uint16_t slave_addr, uint8_t* p_data, uint16_t data_len)
{
    if (!m_iic_enabled[inst])
    {
        return;
    }

    I2C_Transaction transaction;

    transaction.targetAddress = slave_addr;
    transaction.writeBuf      = NULL;
    transaction.writeCount    = 0;
    transaction.readBuf       = p_data;
    transaction.readCount     = data_len;

    I2C_transfer(m_iic_handles[inst], &transaction);
}

void hal_iic_close(uint8_t inst)
{
    if (!m_iic_enabled[inst])
    {
        return;
    }

    m_iic_enabled[inst] = false;
    I2C_close(m_iic_handles[inst]);
}

void hal_iic_cancel(uint8_t inst)
{
    if (!m_iic_enabled[inst])
    {
        return;
    }

    I2C_cancel(m_iic_handles[inst]);
}

void hal_iic_reset(uint8_t inst)
{
    if (m_iic_enabled[inst])
    {
        return;
    }

    /*
     * If IIC is closed and then call I2C_transfer(), then it will stuck at transferComplete semaphore.
     * So we need to post the semaphore to avoid this.
     * Note, this function should be called in another task context. 
     */
    I2CLPF3_Object* p_object = m_iic_handles[inst]->object;

    SemaphoreP_post(&(p_object->transferComplete));
}
