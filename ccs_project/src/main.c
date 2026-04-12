#include "ble_task.h"
#include "hal_os_api.h"
#include "fwk_init.h"
#include "fwk_util.h"
#include "ti_ble_config.h"
#include "log.h"

int main()
{
    fwk_init();

    log_init();

    log_info("\r\n");
    log_info("======================================");
    log_info("Reset by: %x", fwk_get_reset_reason());
    log_info("Build date: %s", fwk_get_build_date());
    log_info("Build time: %s", fwk_get_build_time());
    log_info("Device name: %s", attDeviceName);
    log_info("======================================");

    ble_task_init();

    hal_task_start_scheduler();

    return 0;
}
