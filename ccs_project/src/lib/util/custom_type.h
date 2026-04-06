#ifndef CUSTOM_TYPE_H
#define CUSTOM_TYPE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define OK  0
#define ERR 1

typedef const char* ct_str_t;

typedef struct
{
    uint8_t error;
    size_t code;
} ct_ret_t;

typedef struct
{
    size_t len;
    size_t cap;
    uint8_t* ptr;
} ct_buf_t;

typedef struct
{
    size_t len;
    uint8_t ptr[0];
} ct_pkt_t;

typedef struct
{
    size_t type;
    size_t len;
    uint8_t* value;
} ct_tlv_t;

#endif
