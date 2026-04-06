#ifndef LOG_DEFINE_H
#define LOG_DEFINE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define LOG_BUF_SIZE_OFFSET  40
#define LOG_CAPTION_SIZE_MAX 20    // Caption text size
#define LOG_HEX_COUNT_MAX    100   // Data count of hex dump
#define LOG_PREPEND_SIZE_MAX 20    // Prepend text size
#define LOG_BUFF_SIZE_MAX    (LOG_HEX_COUNT_MAX * 3 + LOG_CAPTION_SIZE_MAX + 12 + LOG_PREPEND_SIZE_MAX)

#if LOG_BUFF_SIZE_MAX < (LOG_HEX_COUNT_MAX * 3 + LOG_CAPTION_SIZE_MAX + 12)
#error "LOG_BUFF_SIZE_MAX is too small"
#endif

#define LOG_BACKEND_UART 0
#define LOG_BACKEND_ITM  1

typedef enum
{
    LOG_LEVEL_TRACE,    /* General trace */
    LOG_LEVEL_DEBUG,    /* General debug */
    LOG_LEVEL_INFO,     /* General info */
    LOG_LEVEL_WARN,     /* General warning, not impact routine */
    LOG_LEVEL_ERROR,    /* General error, impact routine, no need restart */
    LOG_LEVEL_FATAL,    /* Critical error, need restart */
    LOG_LEVEL_NONE,     /* None */
} log_level_t;

#define LOG_LEVEL_ALL LOG_LEVEL_TRACE

typedef void (*log_backend_print_fn)(uint8_t* p_data, uint16_t data_len);

#endif
