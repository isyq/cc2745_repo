#include <stdint.h>

void ble_util_reverse_addr(const uint8_t* p_addr, uint8_t* p_rev_addr)
{
    p_rev_addr[0] = p_addr[5];
    p_rev_addr[1] = p_addr[4];
    p_rev_addr[2] = p_addr[3];
    p_rev_addr[3] = p_addr[2];
    p_rev_addr[4] = p_addr[1];
    p_rev_addr[5] = p_addr[0];
}
