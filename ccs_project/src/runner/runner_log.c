#include "hal_os_api.h"
#include "log.h"

static void runner_task_entry(void *arg)
{
    uint32_t count = 0;
    log_init();

    log_info("Log Task Entry");

    for (;;)
    {
        log_info("Dida, %d", count++);

        hal_delay_ms(1000);
    }
}

void runner_log_task_init(void)
{
    hal_task_create(&(hal_task_param_t){
        .name = "LOG",
        .stack_size = 1536,
        .priority = 5,
        .entry_fn = runner_task_entry,
    });
}
