#include "app_cs_process.h"
#include "ble_cs_config.h"

// Local Tsw. Captured when starting the module by reading local capabilities
uint8_t gLocalTsw = 0;

void ble_cs_distance_init(void)
{
    csCapabilities_t localCaps;

    CSProcess_Start();

    // Clear all sessions DB
    for (uint16_t i = 0; i < MAX_NUM_BLE_CONNS; i++)
    {
        CarNode_clearSession(i);
        CarNode_clearConfigurationParams(i);
    }

    CS_ReadLocalSupportedCapabilities(&localCaps);

    gLocalTsw = localCaps.tSwCap;
}