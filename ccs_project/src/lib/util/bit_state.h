#ifndef BIT_STATE_H
#define BIT_STATE_H

#include <stdbool.h>
#include <stdint.h>

#define BIT_STATE_CODE(x) (0x01 << (x))

#define BIT_STATE_0  0x01
#define BIT_STATE_1  0x02
#define BIT_STATE_2  0x04
#define BIT_STATE_3  0x08
#define BIT_STATE_4  0x10
#define BIT_STATE_5  0x20
#define BIT_STATE_6  0x40
#define BIT_STATE_7  0x80
#define BIT_STATE_8  0x100
#define BIT_STATE_9  0x200
#define BIT_STATE_10 0x400
#define BIT_STATE_11 0x800
#define BIT_STATE_12 0x1000
#define BIT_STATE_13 0x2000
#define BIT_STATE_14 0x4000
#define BIT_STATE_15 0x8000

#define BIT_STATE_MASK_NONE (0x0000)
#define BIT_STATE_MASK_ANY (0xFFFF)

typedef uint16_t bit_state_t;

#define bit_state_match(state, state_mask) (((state) & (state_mask)) != 0)

#endif
