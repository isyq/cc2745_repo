#ifndef Log_H
#define Log_H

#include "log_define.h"

void log_init(void);
void log_set_level(log_level_t level);
void log_print_format(log_level_t level, bool eol, const char* p_format, ...);
void log_print_string_number(log_level_t level, bool eol, const char* p_string, uint32_t number);
void log_print_hex_array(log_level_t level, bool eol, const char* p_caption, uint8_t* p_data, uint16_t data_len);
void log_print_raw_string(log_level_t level, const char* p_string);

#define log_fatal(p_format, ...) log_print_format(LOG_LEVEL_FATAL, 1, p_format, ## __VA_ARGS__)
#define log_error(p_format, ...) log_print_format(LOG_LEVEL_ERROR, 1, p_format, ## __VA_ARGS__)
#define log_warn(p_format, ...)  log_print_format(LOG_LEVEL_WARN, 1, p_format, ## __VA_ARGS__)
#define log_info(p_format, ...)  log_print_format(LOG_LEVEL_INFO, 1, p_format, ## __VA_ARGS__)
#define log_debug(p_format, ...) log_print_format(LOG_LEVEL_DEBUG, 1, p_format, ## __VA_ARGS__)
#define log_trace(p_format, ...) log_print_format(LOG_LEVEL_TRACE, 1, p_format, ## __VA_ARGS__)

#define log_string_number_fatal(p_string, number) log_print_string_number(LOG_LEVEL_FATAL, 1, p_string, number)
#define log_string_number_error(p_string, number) log_print_string_number(LOG_LEVEL_ERROR, 1, p_string, number)
#define log_string_number_warn(p_string, number)  log_print_string_number(LOG_LEVEL_WARN, 1, p_string, number)
#define log_string_number_info(p_string, number)  log_print_string_number(LOG_LEVEL_INFO, 1, p_string, number)
#define log_string_number_debug(p_string, number) log_print_string_number(LOG_LEVEL_DEBUG, 1, p_string, number)
#define log_string_number_trace(p_string, number) log_print_string_number(LOG_LEVEL_TRACE, 1, p_string, number)

#define log_raw_string_fatal(p_string) log_print_raw_string(LOG_LEVEL_FATAL, p_string)
#define log_raw_string_error(p_string) log_print_raw_string(LOG_LEVEL_ERROR, p_string)
#define log_raw_string_warn(p_string)  log_print_raw_string(LOG_LEVEL_WARN, p_string)
#define log_raw_string_info(p_string)  log_print_raw_string(LOG_LEVEL_INFO, p_string)
#define log_raw_string_debug(p_string) log_print_raw_string(LOG_LEVEL_DEBUG, p_string)
#define log_raw_string_trace(p_string) log_print_raw_string(LOG_LEVEL_TRACE, p_string)

#define log_hex_array_fatal(p_caption, p_data, data_len) log_print_hex_array(LOG_LEVEL_FATAL, 1, p_caption, p_data, data_len)
#define log_hex_array_error(p_caption, p_data, data_len) log_print_hex_array(LOG_LEVEL_ERROR, 1, p_caption, p_data, data_len)
#define log_hex_array_warn(p_caption, p_data, data_len)  log_print_hex_array(LOG_LEVEL_WARN, 1, p_caption, p_data, data_len)
#define log_hex_array_info(p_caption, p_data, data_len)  log_print_hex_array(LOG_LEVEL_INFO, 1, p_caption, p_data, data_len)
#define log_hex_array_debug(p_caption, p_data, data_len) log_print_hex_array(LOG_LEVEL_DEBUG, 1, p_caption, p_data, data_len)
#define log_hex_array_trace(p_caption, p_data, data_len) log_print_hex_array(LOG_LEVEL_TRACE, 1, p_caption, p_data, data_len)

#endif
