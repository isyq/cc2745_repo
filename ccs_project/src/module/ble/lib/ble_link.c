#include <string.h>
#include "ble_link.h"
#include "kv_list.h"
#include "hal_os_api.h"

#ifndef CFG_BLE_LINK_COUNT
#define CFG_BLE_LINK_COUNT 4
#endif

#ifndef CFG_BLE_APP_MTU
#define CFG_BLE_APP_MTU 244
#endif

DEF_STATIC_KV_LIST(m_link_list, ble_link_t, CFG_BLE_LINK_COUNT)

static bool is_inited;

void ble_link_init(void)
{
    if (!is_inited)
    {
        is_inited = true;

        kv_list_init(&m_link_list);
    }
}

ble_link_t* ble_link_add(uint16_t handle, uint8_t is_peer_central, uint8_t addr_type, uint8_t* p_adv_addr)
{
    ble_link_init();

    if (m_link_list.count == CFG_BLE_LINK_COUNT)
    {
        return NULL;
    }

    ble_link_t link;

    memset(&link, 0, sizeof(ble_link_t));

    link.handle                = handle;
    link.param.app_mtu         = CFG_BLE_APP_MTU;
    link.param.is_peer_central = is_peer_central;
    link.param.peer_addr_type  = addr_type;
    link.param.conn_timestamp  = hal_get_timestamp64();

    memcpy(link.param.peer_adv_addr_le, p_adv_addr, 6);

    kv_node_t* p_node = kv_list_add(&m_link_list, handle, &link);

    return (ble_link_t*)p_node->data_ptr;
}

ble_link_t* ble_link_get(uint16_t handle)
{
    kv_node_t* p_node = kv_list_search(&m_link_list, handle);

    return (p_node == NULL) ? NULL : (ble_link_t*)p_node->data_ptr;
}

uint8_t ble_link_count(void)
{
    return m_link_list.count;
}
void ble_link_remove(uint16_t handle)
{
    kv_node_t* p_node = kv_list_search(&m_link_list, handle);
    if (p_node != NULL)
    {
        kv_list_remove_node(&m_link_list, p_node);
    }
}

static ble_link_t* find_link_by_addr(uint8_t* p_addr_le)
{
    ble_link_t* p_out_link = NULL;
    kv_node_t* p_node;

    for (uint8_t i = 0; i < m_link_list.count; i++)
    {
        p_node = kv_list_at(&m_link_list, i);

        ble_link_param_t* p_param = &((ble_link_t*)p_node->data_ptr)->param;
        if (memcmp(p_param->peer_adv_addr_le, p_addr_le, 6) == 0)
        {
            p_out_link = (ble_link_t*)p_node->data_ptr;
            break;
        }
    }

    return p_out_link;
}

bool ble_link_exist_by_addr(uint8_t* p_addr_le)
{
    ble_link_t* p_link = find_link_by_addr(p_addr_le);
    return p_link != NULL;
}

bool ble_link_alive_by_addr(uint8_t* p_addr_le)
{
    ble_link_t* p_link = find_link_by_addr(p_addr_le);
    if (p_link != NULL)
    {
        return !p_link->state.disconn_ongoing;
    }
    else
    {
        return false;
    }
}

bool ble_link_alive_by_handle(uint16_t handle)
{
    ble_link_t* p_link = ble_link_get(handle);
    if (p_link != NULL)
    {
        return !p_link->state.disconn_ongoing;
    }
    else
    {
        return false;
    }
}

uint64_t ble_link_alive_time(uint16_t handle)
{
    ble_link_t* p_link = ble_link_get(handle);
    if (p_link != NULL)
    {
        return hal_get_timestamp64() - p_link->param.conn_timestamp;
    }
    else
    {
        return 0;
    }
}
