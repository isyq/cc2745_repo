#ifndef HAL_OS_API_H
#define HAL_OS_API_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <timers.h>

#define OSAL_MALLOC malloc
#define OSAL_FREE   free

typedef void (*hal_task_entry_fn)(void* arg);

typedef struct
{
    const char* name;
    uint32_t stack_size;
    uint8_t priority;       // Higher number --> lower priority
    hal_task_entry_fn entry_fn;
    void* arg;
} hal_task_param_t;

typedef struct
{
    void* handle;
} hal_task_t;

typedef struct
{
    void* handle;
} hal_mutex_t;

typedef struct
{
    void* handle;
} hal_event_t;

typedef struct
{
    void* handle;
} hal_timer_t;

typedef struct
{
    char* name;
    bool repeat;
    uint32_t period;
    uintptr_t arg;
    TimerCallbackFunction_t callback;
} hal_timer_param_t;

hal_task_t hal_task_create(hal_task_param_t* p_param);
char* hal_task_get_name(void);
void hal_task_start_scheduler(void);
void hal_task_suspend_self(void);

void hal_mutex_init(hal_mutex_t* p_mutex);
void hal_mutex_lock(hal_mutex_t* p_mutex);
void hal_mutex_unlock(hal_mutex_t* p_mutex);

void hal_event_create(hal_event_t* p_event);
void hal_event_wait(hal_event_t* p_event, uint32_t event_mask);
void hal_event_post(hal_event_t* p_event, uint32_t event_bits);
void hal_event_get(hal_event_t* p_event);
void hal_event_clear(hal_event_t* p_event);

void hal_delay_ms(uint16_t delay_ms);

uint32_t hal_get_timestamp32(void);
uint64_t hal_get_timestamp64(void);

void hal_timer_create(hal_timer_t* p_timer, hal_timer_param_t* p_param);
void hal_timer_start(hal_timer_t* p_timer, uint32_t delay_ms);
void hal_timer_stop(hal_timer_t* p_timer);
void hal_timer_alive(hal_timer_t* p_timer);

#endif
