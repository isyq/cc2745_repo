#include "ble_user_event.h"
#include "log.h"
#include "ble_stack_task.h"
#include "ble_adv.h"
#include "ble_scan.h"

#if defined CHANNEL_SOUNDING
#include "ble_cs.h"
#endif

void HANDLER_NAME(BLE_USER_EVENT_STACK_READY)(char* p_data)
{
    log_info("BLE stack ready");

#if defined CHANNEL_SOUNDING
    // ble_cs_init();
#endif

#if (HOST_CONFIG & PERIPHERAL_CFG)
    ble_adv_init();
    ble_adv_start();
#endif

// #if (HOST_CONFIG & CENTRAL_CFG)
//     ble_scan_init();
//     ble_scan_start();
// #endif
}

void HANDLER_NAME(BLE_USER_EVENT_IN_CONNECTION)(char* p_data)
{
    log_info("BLE in connection");
}

void HANDLER_NAME(BLE_USER_EVENT_NO_CONNECTION)(char* p_data)
{
    log_info("BLE no connection");
}
