#include "util_tool.h"
#include "ble_user_event.h"
#include "bleapputil_api.h"

DEF_WEAK_BLE_USER_EVENT_HANDLER(HANDLER_NAME(BLE_USER_EVENT_STACK_READY));
DEF_WEAK_BLE_USER_EVENT_HANDLER(HANDLER_NAME(BLE_USER_EVENT_IN_CONNECTION));
DEF_WEAK_BLE_USER_EVENT_HANDLER(HANDLER_NAME(BLE_USER_EVENT_NO_CONNECTION));

typedef struct
{
    uint8_t event;
    InvokeFromBLEAppUtilContext_t callback;
} ble_user_event_callback_pair_t;

static ble_user_event_callback_pair_t m_event_pairs[] = 
{
    {BLE_USER_EVENT_STACK_READY, HANDLER_NAME(BLE_USER_EVENT_STACK_READY)},
    {BLE_USER_EVENT_IN_CONNECTION, HANDLER_NAME(BLE_USER_EVENT_IN_CONNECTION)},
    {BLE_USER_EVENT_NO_CONNECTION, HANDLER_NAME(BLE_USER_EVENT_NO_CONNECTION)}
};

void ble_user_event_start(uint8_t event, uint8_t* p_data, uint16_t data_len)
{
    ble_user_data_t* p_event = BLEAppUtil_malloc(sizeof(ble_user_data_t) + data_len);

    if (p_event == NULL)
    {
        return;
    }

    p_event->data_len = data_len;

    if (data_len > 0 && p_data != NULL)
    {
        memcpy(p_event->p_data, p_data, data_len);
    }

    BLEAppUtil_invokeFunction(m_event_pairs[event].callback, (char*)p_event);
}

void ble_user_event_stop(ble_user_data_t* p_event)
{
    BLEAppUtil_free(p_event);
}
