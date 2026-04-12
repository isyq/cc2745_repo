#include "ti_ble_config.h"
#include "ble_host_init.h"
#include "ble_dev_info.h"

static void init_ble_dev_info(void)
{
    memset(&g_ble_dev_info, 0, sizeof(g_ble_dev_info));

    g_ble_dev_info.addr_type       = DEFAULT_ADDRESS_MODE;
    g_ble_dev_info.device_name     = attDeviceName;
    g_ble_dev_info.device_name_len = strlen((char*)attDeviceName);
}

void ble_task_init(void)
{
    init_ble_dev_info();

    ble_host_init();
}
