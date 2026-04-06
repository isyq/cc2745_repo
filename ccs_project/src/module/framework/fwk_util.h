#ifndef FRAEMWORK_UTIL_H
#define FRAEMWORK_UTIL_H

const char* fwk_get_build_date(void);
const char* fwk_get_build_time(void);
uint32_t fwk_get_reset_reason(void);
void fwk_reset_system(void);

#endif
