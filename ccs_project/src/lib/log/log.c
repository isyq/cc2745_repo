#include "log_define.h"
#include "log_format.h"
#include "log_backend.h"
#include "hal_os_api.h"
#include "util_tool.h"

#ifndef CFG_LOG_LEVEL
#define CFG_LOG_LEVEL LOG_LEVEL_INFO
#endif

#ifndef CFG_LOG_PREFIX_SYMBOL_LEVEL
#define CFG_LOG_PREFIX_SYMBOL_LEVEL LOG_LEVEL_DEBUG
#endif

#ifndef CFG_LOG_BACKEND
#define CFG_LOG_BACKEND LOG_BACKEND_ITM
#endif

#ifndef CFG_LOG_TIME_TICK_ENABLED
#define CFG_LOG_TIME_TICK_ENABLED 0
#endif

#ifndef CFG_LOG_TASK_NAME_ENABLED
#define CFG_LOG_TASK_NAME_ENABLED 1
#endif

#ifndef CFG_LOG_LEVEL_SYMBOL_ENABLED
#define CFG_LOG_LEVEL_SYMBOL_ENABLED 0
#endif

static log_level_t m_log_level;
static hal_mutex_t m_log_mutex;

#define INIT_MUTEX() hal_mutex_init(&m_log_mutex)
#define PEND_MUTEX() hal_mutex_lock(&m_log_mutex)
#define POST_MUTEX() hal_mutex_unlock(&m_log_mutex)

void log_init(void)
{
    static bool is_init = false;

    if (!is_init)
    {
        is_init = true;

        m_log_level = CFG_LOG_LEVEL;

        log_backend_init(CFG_LOG_BACKEND);

        INIT_MUTEX();
    }
}

void log_set_level(log_level_t level)
{
    m_log_level = level;
}

static uint16_t prepend_state_info(log_level_t level, uint8_t* p_buffer)
{
    uint16_t strLen = 0;

#if CFG_LOG_TIME_TICK_ENABLED > 0
    uint32_t timeMs = time_util_get_minisecond();
    strLen += log_format_prepend_tick_time(timeMs, p_buffer, strLen);
#endif

#if CFG_LOG_TASK_NAME_ENABLED > 0
    const char* task_name = hal_task_get_name();
    strLen += log_format_prepend_task_name(task_name, p_buffer, strLen);
#endif

#if CFG_LOG_LEVEL_SYMBOL_ENABLED > 0
    if (level <= CFG_LOG_PREFIX_SYMBOL_LEVEL)
    {
        strLen += log_format_prepend_level_symbol(level, p_buffer, strLen);
    }
#endif

    return strLen;
}

void log_print_format(log_level_t level, bool eol, const char* p_format, ...)
{
    RETURN_VOID_IF(p_format == NULL);
    RETURN_VOID_IF(level < m_log_level);

    PEND_MUTEX();

    uint8_t buffer[LOG_BUFF_SIZE_MAX + LOG_BUF_SIZE_OFFSET];

    va_list args;
    va_start(args, p_format);

    uint16_t len = 0;

    len = prepend_state_info(level, buffer);
    len += log_format_string_args(p_format, args, buffer, len);
    if (eol)
    {
        len = log_format_append_eol(buffer, len);
    }
    va_end(args);

    log_backend_print(level, buffer, len);

    POST_MUTEX();
}

void log_print_string_number(log_level_t level, bool eol, const char* p_string, uint32_t number)
{
    RETURN_VOID_IF(p_string == NULL);
    RETURN_VOID_IF(level < m_log_level);

    PEND_MUTEX();

    uint8_t buffer[LOG_BUFF_SIZE_MAX + LOG_BUF_SIZE_OFFSET];

    uint16_t len = 0;

    len = prepend_state_info(level, buffer);

    len += log_format_string_number(p_string, number, buffer, len);
    if (eol)
    {
        len = log_format_append_eol(buffer, len);
    }
    log_backend_print(level, buffer, len);

    POST_MUTEX();
}

void log_print_hex_array(log_level_t level, bool eol, const char* p_caption, uint8_t* p_data, uint16_t data_len)
{
    RETURN_VOID_IF(p_data == NULL);
    RETURN_VOID_IF(level < m_log_level);

    PEND_MUTEX();

    uint8_t buffer[LOG_BUFF_SIZE_MAX + LOG_BUF_SIZE_OFFSET];

    uint16_t len = 0;

    len = prepend_state_info(level, buffer);

    len += log_format_hex_array(p_caption, p_data, data_len, buffer, len);
    if (eol)
    {
        len = log_format_append_eol(buffer, len);
    }
    log_backend_print(level, buffer, len);

    POST_MUTEX();
}

void log_print_raw_string(log_level_t level, const char* p_string)
{
    RETURN_VOID_IF(p_string == NULL);
    RETURN_VOID_IF(level < m_log_level);

    PEND_MUTEX();

    uint16_t len = strlen(p_string);
    log_backend_print(level, (char*)p_string, len);

    POST_MUTEX();
}
