#include "bleapputil_api.h"

void ble_cs_init(void)
{
    BLEAppUtil_registerCsCB();

#if defined RANGING_SERVER
    #include "ble_ras_server.h"

    ble_ras_server_init();
#endif

#if defined RANGING_CLIENT
    #include "ble_ras_client.h"

    ble_ras_client_init();
#endif
}
