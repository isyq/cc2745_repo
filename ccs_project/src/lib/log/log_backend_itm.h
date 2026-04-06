#ifndef LOG_BACKEND_ITM_H
#define LOG_BACKEND_ITM_H

#include <stdint.h>

void log_backend_itm_init(void);
void log_backend_itm_write(uint8_t* p_data, uint16_t data_len);


#endif
