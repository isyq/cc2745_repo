#include "ti_drivers_config.h"
#include "hal_os_api.h"
#include "hal_drv_api.h"

static void button_isr_left(uint8_t pin)
{
    hal_gpio_toggle_pin(CONFIG_GPIO_LED_GREEN);
}

static void runner_task_entry(void *arg)
{
    hal_gpio_create_in_pin(CONFIG_GPIO_BUTTON_0_INPUT, HAL_GPIO_IN_PULL_UP, HAL_GPIO_INT_FALLING, button_isr_left);
    hal_gpio_create_out_pin(CONFIG_GPIO_LED_GREEN, HAL_GPIO_OUT_PULL_STD, 0);

    hal_gpio_enable_interrupt(CONFIG_GPIO_BUTTON_0_INPUT);

    for (;;)
    {
        hal_task_suspend_self();
    }
}

void runner_gpio_task_init(void)
{
    hal_task_create(&(hal_task_param_t){
        .name = "GPIO",
        .stack_size = 1024,
        .priority = 5,
        .entry_fn = runner_task_entry,
    });
}
