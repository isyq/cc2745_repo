#ifndef RETURN_CODE_H
#define RETURN_CODE_H

/* Include most common used standard libraries */
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define RET_OK                    0

#define RET_ERR_FAIL              0xFF00
#define RET_ERR_FAIL_UNKNOWN      (RET_ERR_FAIL + 1)
#define RET_ERR_FAIL_NOT_IMPL     (RET_ERR_FAIL + 2)
#define RET_ERR_FAIL_NOT_SUPPORT  (RET_ERR_FAIL + 3)

#define RET_ERR_PARAM             0xFF10
#define RET_ERR_PARAM_LEN_ZERO    (RET_ERR_PARAM + 1)
#define RET_ERR_PARAM_LEN_SMALL   (RET_ERR_PARAM + 2)
#define RET_ERR_PARAM_LEN_LARGE   (RET_ERR_PARAM + 3)
#define RET_ERR_PARAM_VALUE_ZERO  (RET_ERR_PARAM + 4)
#define RET_ERR_PARAM_VALUE_NULL  (RET_ERR_PARAM + 5)
#define RET_ERR_PARAM_VALUE_SMALL (RET_ERR_PARAM + 6)
#define RET_ERR_PARAM_VALUE_LARGE (RET_ERR_PARAM + 7)
#define RET_ERR_PARAM_OUT_RANGE   (RET_ERR_PARAM + 8)

#define RET_ERR_STATE             0xFF20
#define RET_ERR_STATE_TIMEOUT     (RET_ERR_STATE + 1)
#define RET_ERR_STATE_BUSY        (RET_ERR_STATE + 2)
#define RET_ERR_STATE_ONGOING     (RET_ERR_STATE + 3)
#define RET_ERR_STATE_NOT_READY   (RET_ERR_STATE + 4)
#define RET_ERR_STATE_NOT_MATCH   (RET_ERR_STATE + 5)

#define RET_ERR_OBJECT            0xFF30
#define RET_ERR_OBJECT_NOT_FOUND  (RET_ERR_OBJECT + 1)
#define RET_ERR_OBJECT_EXISTED    (RET_ERR_OBJECT + 2)
#define RET_ERR_OBJECT_UNKNOWN    (RET_ERR_OBJECT + 3)

#define RET_ERR_MEMORY            0xFF40
#define RET_ERR_MEMORY_LACK       (RET_ERR_MEMORY + 1)
#define RET_ERR_MEMORY_FULL       (RET_ERR_MEMORY + 2)

#define RET_ERR_FORMAT            0xFF50
#define RET_ERR_FORMAT_LEN        (RET_ERR_FORMAT + 1)
#define RET_ERR_FORMAT_CRC        (RET_ERR_FORMAT + 2)

typedef uint16_t ret_t;

#define RET_CODE(x) (x) == 0 ? 0 : (ret_t)(x)


#endif
