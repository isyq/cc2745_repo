#include "msg_bus.h"
#include "fwk_event.h"
#include "log.h"
#include "util_tool.h"
#include "ble_user_event.h"
#include "ble_stack_task.h"

void HANDLER_NAME(FWK_EVENT_INIT_STACK)(void* p_data, uintptr_t value)
{
    log_info("Start init stack");

    ble_stack_task_init();
}


void HANDLER_NAME(FWK_EVENT_STACK_READY)(void* p_data, uintptr_t value)
{
    log_info("Init stack done");

    ble_user_event_start(BLE_USER_EVENT_STACK_READY, NULL, 0);
}

void HANDLER_NAME(FWK_EVENT_ENTER_SLEEP)(void* p_data, uintptr_t value)
{
    log_info("Enter sleep");
}

void HANDLER_NAME(FWK_EVENT_EXIT_SLEEP)(void* p_data, uintptr_t value)
{
    log_info("Exit sleep");
}
