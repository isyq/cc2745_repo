#ifndef MSG_BUS_H
#define MSG_BUS_H

#include "kv_list.h"
#include "linked_list.h"
#include "hal_os_api.h"

typedef void (*mbus_topic_fn)(void* p_data, uintptr_t value);

typedef struct
{
    uint32_t id;
    const char* name;
    mbus_topic_fn callback;
} mbus_topic_t;

typedef struct
{
    uint32_t id;
    const char* name;
    kv_list_t* topic_list;
} mbus_chann_t;

typedef struct
{
    uint32_t chann_id;
    union
    {
        uint32_t topic_id;
        mbus_topic_fn callback;
    };
    uint8_t* data_ptr;
    uintptr_t value;
} mbus_msg_t;

typedef struct
{
    mbus_chann_t* chann_array;
    uint32_t chann_count;
    linked_list_t msg_queue;
    hal_event_t event;
} mbus_t;

void mbus_init(mbus_t* p_bus, mbus_chann_t* p_chann_array, uint32_t chann_count);
void mbus_register_topic(mbus_t* p_bus, uint32_t chann_id, mbus_topic_t* p_topic);
void mbus_produce_message(mbus_t* p_bus, uint32_t chann, uint32_t topic, uint8_t* p_data, uintptr_t value);
void mbus_produce_call(mbus_t* p_bus, mbus_topic_fn callback, uint8_t* p_data, uintptr_t value);
void mbus_consume_call(mbus_t* p_bus);

#endif
