#include "hal_os_api.h"
#include "fwk_init.h"
#include "fwk_task.h"
#include "runner_task_list.h"

int main()
{
    fwk_init();

    fwk_task_init();
    // runner_task_init();

    hal_task_scheduler_start();

    return 0;
}
