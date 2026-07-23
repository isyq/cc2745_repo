#ifndef BLE_CS_CONFIG_H
#define BLE_CS_CONFIG_H


#define CS_RANGING_PCT_ARRAY_SIZE             75      // CS Ranging library PCT results array size for 1 antenna path
#define CS_RANGING_MAX_ANT_PATHS              4       // CS Ranging library maximum number of antenna paths

// CS Ranging library PCT results array size for all antenna paths
#define CS_RANGING_PCT_ARRAY_SIZE_PATHS      (CS_RANGING_PCT_ARRAY_SIZE * CS_RANGING_MAX_ANT_PATHS)

#ifndef MAX_NUM_BLE_CONNS
#define MAX_NUM_BLE_CONNS 4
#endif

#endif
