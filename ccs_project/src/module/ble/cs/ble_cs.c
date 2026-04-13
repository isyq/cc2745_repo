#include "ble_cs_client.h"
#include "ble_cs_server.h"

void ble_cs_init(void)
{
    BLEAppUtil_registerCsCB();

#if defined RANGING_SERVER
    ble_cs_server_init();
#endif

#if defined RANGING_CLIENT
    ble_cs_client_init();
#endif
}
