#include "msg_bus.h"
#include "fwk_mbus.h"
#include "log.h"
#include "ble_stack_task.h"

void MBUS_TOPIC_HANDLER_NAME(FWK_EVENT_INIT_STACK)(void* p_data, uintptr_t value)
{
    log_info("Start init stack");

    ble_stack_task_init();
}

void MBUS_TOPIC_HANDLER_NAME(FWK_EVENT_STACK_READY)(void* p_data, uintptr_t value)
{
    log_info("Init stack done");



}

void MBUS_TOPIC_HANDLER_NAME(FWK_EVENT_ENTER_SLEEP)(void* p_data, uintptr_t value)
{
    log_info("Enter sleep");
}

void MBUS_TOPIC_HANDLER_NAME(FWK_EVENT_EXIT_SLEEP)(void* p_data, uintptr_t value)
{
    log_info("Exit sleep");
}
