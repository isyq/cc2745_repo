#ifndef LOG_BACKEND_UART_H
#define LOG_BACKEND_UART_H

#include <stdint.h>

void log_backend_uart_init(void);
void log_backend_uart_write(uint8_t* p_data, uint16_t data_len);

#endif
