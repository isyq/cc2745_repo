#ifndef LOG_FORMAT_H
#define LOG_FORMAT_H

#include <stdint.h>
#include <stdarg.h>

uint16_t log_format_string_args(const char* p_format, va_list args, uint8_t* p_buffer, uint16_t offset);
uint16_t log_format_string_number(const char* p_string, uint32_t number, uint8_t* p_buffer, uint16_t offset);
uint16_t log_format_hex_array(const char* p_caption, uint8_t* p_data, uint16_t data_len, uint8_t* p_buffer, uint16_t offset);
uint16_t log_format_number(uint32_t number, uint8_t* p_buffer);
uint16_t log_format_append_eol(uint8_t* p_buffer, uint16_t bufferLen);
uint16_t log_format_prepend_tick_time(uint32_t timestamp, uint8_t* p_buffer, uint16_t offset);
uint16_t log_format_prepend_task_name(const char* p_name, uint8_t* p_buffer, uint16_t offset);
uint16_t log_format_prepend_level_symbol(uint8_t level, uint8_t* p_buffer, uint16_t offset);

#endif
