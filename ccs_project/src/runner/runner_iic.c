#include "hal_os_api.h"
#include "hal_drv_api.h"
#include "log.h"

#define IIC_INST 0

static void runner_task_entry(void* arg)
{
    uint32_t count = 0;
    hal_iic_init(IIC_INST, 1, NULL);

    for (;;)
    {
        hal_iic_send_timeout(IIC_INST, 0x1122, (uint8_t*)"Hello\r\n", 7, 1000);
        log_info("IIC send complete, %d", count++);

        hal_delay_ms(1000);
    }
}

void runner_iic_create_task(void)
{
    hal_task_create(&(hal_task_param_t){
        .name = "IIC",
        .stack_size = 1024,
        .priority = 5,
        .entry_fn = runner_task_entry,
    });
}

