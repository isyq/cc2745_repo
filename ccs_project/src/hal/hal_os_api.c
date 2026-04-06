#include <FreeRTOS.h>
#include <task.h>
#include <semaphore.h>
#include <timers.h>
#include <ti\drivers\dpl\TimestampP.h>
#include "hal_os_api.h"
#include "util_tool.h"

#define FREERTOS_SEM_PTR(x) (sem_t*)&((x)->handle)

void hal_mutex_init(hal_mutex_t* p_mutex)
{
    sem_init(FREERTOS_SEM_PTR(p_mutex), 0, 1);
}

void hal_mutex_lock(hal_mutex_t* p_mutex)
{
    sem_wait(FREERTOS_SEM_PTR(p_mutex));
}

void hal_mutex_unlock(hal_mutex_t* p_mutex)
{
    sem_post(FREERTOS_SEM_PTR(p_mutex));
}

void hal_event_create(hal_event_t* p_event)
{

}

void hal_event_wait(hal_event_t* p_event, uint32_t event_mask)
{

}

void hal_event_post(hal_event_t* p_event, uint32_t event_bits)
{

}

void hal_event_get(hal_event_t* p_event)
{

}

void hal_event_clear(hal_event_t* p_event)
{

}

hal_task_t hal_task_create(hal_task_param_t* p_param)
{
    /* If xTaskCreate fails, pHandle will remain NULL */
    TaskHandle_t handle = NULL;

    /* Copy from pthread_create in pthread.c */
    uint16_t stack_size_in_words = p_param->stack_size / sizeof(portSTACK_TYPE);
    xTaskCreate(p_param->entry_fn, p_param->name, stack_size_in_words, p_param->arg, p_param->priority, &handle);

    return (hal_task_t){.handle = handle};
}

char* hal_task_get_name(void)
{
    char* name;
    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();

    if (likely((task_handle != NULL)))
    {
        name = pcTaskGetName(NULL);

        if (unlikely(name == NULL))
        {
            name = "Unknown";
        }
    }
    else
    {
        name = "Host";
    }

    return name;
}

void hal_task_start_scheduler(void)
{
    vTaskStartScheduler();
}

void hal_task_suspend_self(void)
{
    vTaskSuspend(NULL);
}

void hal_delay_ms(uint16_t delay_ms)
{
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}

uint32_t hal_get_timestamp32(void)
{
    return TimestampP_getNative32();
}

uint64_t hal_get_timestamp64(void)
{
    return TimestampP_getNative64();
}

void hal_timer_create(hal_timer_t* p_timer, hal_timer_param_t* p_param)
{
    bool is_auto_load = p_param->repeat ? pdTRUE : pdFALSE;
    uint32_t period_ticks = pdMS_TO_TICKS(p_param->period);

    p_timer->handle = xTimerCreate(p_param->name, period_ticks, is_auto_load, (void*)p_param->arg, p_param->callback);
}

void hal_timer_start(hal_timer_t* p_timer, uint32_t delay_ms)
{
    xTimerStart(p_timer->handle, pdMS_TO_TICKS(delay_ms));
}

void hal_timer_stop(hal_timer_t* p_timer)
{
    xTimerStop(p_timer->handle, 0);
}

void hal_timer_alive(hal_timer_t* p_timer)
{
    xTimerIsTimerActive(p_timer->handle);
}
