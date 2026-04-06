#ifndef LOG_BACKEND_H
#define LOG_BACKEND_H

#include "log_define.h"

void log_backend_init(uint8_t backend);
void log_backend_print(log_level_t level, void* p_data, uint16_t data_len);

#endif
