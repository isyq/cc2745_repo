#ifndef UTIL_TOOL_H
#define UTIL_TOOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <assert.h>

/* Only used at the beginning of a function */
#define RETURN_VOID_IF(cond) if (cond) return;
#define RETURN_NULL_IF(cond) if (cond) return NULL;
#define RETURN_IF(cond, err) if (cond) return (err);

#define ALIGN_SIZE alignof(max_align_t)
#define ALIGN_UP(x, align)   (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

/* x % y == x & (y - 1) when y == 2^n */
#define POWER2_GET_MOD(x, y) ((x) & ((y) - 1))
/* x & (x - 1) == 0 when x == 2^n */
#define POWER2_CHECK(x) (((x) & ((x) - 1)) == 0)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({                       \
        const __typeof__( ((type*)0)->member ) * __mptr = (ptr); \
        (type*)( (char*)__mptr - offsetof(type, member) );})

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define ATOMIC_GET_VALUE(p_value)        atomic_load(p_value)
#define ATOMIC_SET_VALUE(p_value, value) atomic_store(p_value, value)
#define ATOMIC_INC_VALUE(p_value)        atomic_fetch_add((p_value), 1)
#define ATOMIC_DEC_VALUE(p_value)        atomic_fetch_sub((p_value), 1)
#define ATOMIC_SET_TRUE(p_value)         atomic_store((p_value), true)
#define ATOMIC_SET_FALSE(p_value)        atomic_store((p_value), false)

#define ATOMIC_LOCK_FLAG(p_flag)   while (atomic_flag_test_and_set(p_flag))
#define ATOMIC_UNLOCK_FLAG(p_flag) atomic_flag_clear(p_flag)

#define ROUND_INC_U8(value)  (value < 0xFF)       ? (value + 1) : 0
#define ROUND_INC_U16(value) (value < 0xFFFF)     ? (value + 1) : 0
#define ROUND_INC_U32(value) (value < 0xFFFFFFFF) ? (value + 1) : 0

#define ROUND_DEC_U8(value)  (value > 0) ? (value - 1) : 0xFF
#define ROUND_DEC_U16(value) (value > 0) ? (value - 1) : 0xFFFF
#define ROUND_DEC_U32(value) (value > 0) ? (value - 1) : 0xFFFFFFFF

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define GET_MIN(n, m) ((n) < (m)) ? (n) : (m)
#define GET_MAX(n, m) ((n) < (m)) ? (m) : (n)

#define U16_DECODE_BE(array) (uint16_t)((((array)[0] << 8) & 0xFF00) + (array)[1])
#define U16_DECODE_LE(array) (uint16_t)((((array)[1] << 8) & 0xFF00) + (array)[0])
#define U32_DECODE_BE(array) (uint32_t)(U16_DECODE_BE(array) << 16) + U16_DECODE_BE((array + 2))
#define U32_DECODE_LE(array) (uint32_t)(U16_DECODE_LE(array) << 16) + U16_DECODE_LE((array + 2))

#define U16_ENCODE_BE(val, array)                      \
        do                                             \
        {                                              \
            (array)[0] = (uint8_t)((val >> 8) & 0xFF); \
            (array)[1] = (uint8_t)(val & 0xFF);        \
        } while (0)

#define U16_ENCODE_LE(val, array)                      \
        do                                             \
        {                                              \
            (array)[1] = (uint8_t)((val >> 8) & 0xFF); \
            (array)[0] = (uint8_t)(val & 0xFF);        \
        } while (0)

#define U32_ENCODE_BE(val, array)                       \
        do                                              \
        {                                               \
            (array)[0] = (uint8_t)((val >> 24) & 0xFF); \
            (array)[1] = (uint8_t)((val >> 16) & 0xFF); \
            (array)[2] = (uint8_t)((val >> 8) & 0xFF);  \
            (array)[3] = (uint8_t)(val & 0xFF);         \
        } while (0)

#define U32_ENCODE_LE(val, array)                       \
        do                                              \
        {                                               \
            (array)[3] = (uint8_t)((val >> 24) & 0xFF); \
            (array)[2] = (uint8_t)((val >> 16) & 0xFF); \
            (array)[1] = (uint8_t)((val >> 8) & 0xFF);  \
            (array)[0] = (uint8_t)(val & 0xFF);         \
        } while (0)

#define ATTR_WEAK       __attribute__((__weak__))
#define ATTR_ALIGN(x)   __attribute__((aligned(x)))
#define ATTR_PACKED     __attribute__((packed))

#endif
