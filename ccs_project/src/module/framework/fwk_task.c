#include "ti_drivers_config.h"
#include "hal_os_api.h"
#include "fwk_util.h"
#include "log.h"
#include "ti_ble_config.h"
#include "fwk_event.h"

static void print_welcome(void)
{
    log_info("\r\n");
    log_info("======================================");
    log_info("Reset by: %x", fwk_get_reset_reason());
    log_info("Build date: %s", fwk_get_build_date());
    log_info("Build time: %s", fwk_get_build_time());
    log_info("Device name: %s", attDeviceName);
    log_info("======================================");
}

static void runner_task_entry(void* arg)
{
    log_init();
    print_welcome();

    fwk_event_init();

    fwk_event_post(FWK_EVENT_INIT_STACK);

    for (;;)
    {
        fwk_event_pend();
    }
}

void fwk_task_init(void)
{
    hal_task_create(&(hal_task_param_t){
        .name       = "FWK",
        .stack_size = 2048,
        .priority   = 5,
        .entry_fn   = runner_task_entry,
    });
}
