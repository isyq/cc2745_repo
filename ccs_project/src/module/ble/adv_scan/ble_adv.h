#ifndef BLE_ADV_H
#define BLE_ADV_H

#include <stdint.h>

void ble_adv_init(void);
void ble_adv_start(void);
void ble_adv_stop(void);
void ble_adv_update(uint8_t set_id, uint8_t* p_data, uint8_t len);

#endif
