#ifndef BLE_LINK_H
#define BLE_LINK_H

#include <stdint.h>
#include <stdbool.h>

#define INVALID_LINK_HANDLE    0xFFFF

typedef struct
{
    bool is_self_central;
    bool is_self_cs_initiator;
    bool is_self_ras_client;

    uint8_t peer_addr_type;
    uint8_t peer_adv_addr_le[6];   // Little endian
    uint8_t peer_mac_addr_le[6];   // Little endian
    uint8_t peer_irk[16];

    uint8_t ltk[16];

    uint64_t conn_timestamp;

    uint8_t app_mtu;
    uint8_t phy_type;

    uint16_t llcp_interval;
    uint16_t llcp_timeout;
    uint16_t llcp_latency;

    uint16_t rx_count;
    uint16_t tx_count;
} ble_link_param_t;

typedef struct
{
    bool cccd_enabled[4];
    bool l2cap_established;
    bool bonding_flag;
    bool use_oob_flag;
    
    bool disconn_ongoing;
    bool phy_proc_ongoing;
    bool llcp_proc_ongoing;
    bool mtu_proc_ongoing;
    bool pair_proc_ongoing;
    bool encr_proc_ongoing;
    bool bond_proc_ongoing;

    bool phy_proc_done;
    bool llcp_proc_done;
    bool mtu_proc_done;
    bool pair_proc_done;
    bool encr_proc_done;
    bool bond_proc_done;
} ble_link_state_t;

typedef struct
{
    uint16_t handle;
    ble_link_param_t param;
    ble_link_state_t state;
} ble_link_t;

void ble_link_init(void);
ble_link_t* ble_link_add(uint16_t handle, uint8_t is_central, uint8_t addr_type, uint8_t* p_adv_addr);
ble_link_t* ble_link_get(uint16_t handle);
uint8_t ble_link_count(void);
void ble_link_remove(uint16_t handle);
bool ble_link_exist_by_addr(uint8_t* p_addr_le);
bool ble_link_alive_by_addr(uint8_t* p_addr_le);
bool ble_link_alive_by_handle(uint16_t handle);
uint64_t ble_link_alive_time(uint16_t handle);

#endif
