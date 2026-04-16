#include <FreeRTOS.h>
#include <task.h>
#include "ti/ble/stack_util/health_toolkit/assert.h"
#include "fwk_init.h"
#include "log.h"

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    fwk_assert_handler(HAL_ASSERT_CAUSE_STACK_OVERFLOW_ERROR, 0);
}

void freertos_hardfault_callback(void)
{
    for (;;)
    {;}
}