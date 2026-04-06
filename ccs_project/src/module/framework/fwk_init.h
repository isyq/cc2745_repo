#ifndef FRAMEWORK_INIT_H
#define FRAMEWORK_INIT_H

#include <stdint.h>

void fwk_init(void);
void fwk_assert_handler(uint8_t assertCause, uint8_t assertSubcause);

#endif
