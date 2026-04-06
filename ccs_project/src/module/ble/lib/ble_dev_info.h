#ifndef BLE_DEV_INFO_H
#define BLE_DEV_INFO_H

#include <stdint.h>

typedef struct
{
    uint8_t device_name_len;
    uint8_t* device_name;

    uint8_t addr_type;
    uint8_t adv_addr_le[6];  /* Resolvable private address in little endian */
    uint8_t mac_addr_le[6];  /* ID address in little endian, the format in memory layout */

    uint8_t irk[16];         /* IRK in little endian */

} ble_dev_info_t;

static ble_dev_info_t g_ble_dev_info;


#endif