#ifndef BLE_CHAR_H
#define BLE_CHAR_H

#include <stdint.h>
#include <stdbool.h>

#define UUID_SIZE_SIG 2
#define UUID_SIZE_VS  16

/* 0000XXXX-0000-1000-8000–00805f9b34fb */
#define BLE_SIG_UUID_BASE(high, low)                                                                  \
    {                                                                                                 \
        0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, low, high, 0x00, 0x00 \
    }

typedef enum
{
    BLE_CHANN_NONE,
    BLE_CHANN_DK_AUTH,
    BLE_CHANN_DK_AUXI,
    BLE_CHANN_CHARGER,
    BLE_CHANN_BLE_CCC,
    BLE_CHANN_FOB_KEY,
} bleChann_t;

typedef struct
{
    uint16_t connHandle;
    bleChann_t channId;
    uint8_t* pData;
    uint8_t dataLen;
} writeCharEventData_t;

typedef struct
{
    uint16_t connHandle;
    bleChann_t channId;
    bool cccdReady;
} writeCccdEventData_t;

#endif