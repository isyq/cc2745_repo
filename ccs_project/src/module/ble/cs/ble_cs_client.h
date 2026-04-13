#ifndef BLE_CS_CLIENT_H
#define BLE_CS_CLIENT_H

#include <stdint.h>

void ble_cs_client_init(void);
uint8_t ble_cs_client_enable(uint16_t connHandle, uint8_t enableMode);

#endif
