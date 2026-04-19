#include "msg_bus.h"

#define MBUS_NEW_EVENT_ID  0x00000001
#define MBUS_CALL_GROUP_ID 0xFF

void mbus_init(mbus_t* p_bus, mbus_chann_t* p_chann_array, uint32_t chann_count)
{
    linked_list_init_for_heap(&p_bus->msg_queue);
    hal_event_create(&p_bus->event);

    p_bus->chann_array = p_chann_array;
    p_bus->chann_count = chann_count;

    for (uint32_t i = 0; i < chann_count; i++)
    {
        kv_list_init(p_chann_array[i].topic_list);
    }
}

void mbus_register_topic(mbus_t* p_bus, uint32_t chann_id, mbus_topic_t* p_topic)
{
    mbus_chann_t* p_chann = &p_bus->chann_array[chann_id];

    kv_list_add(p_chann->topic_list, p_topic->id, p_topic);
}

void mbus_sort_topic(mbus_t* p_bus)
{
    for (uint32_t i = 0; i < p_bus->chann_count; i++)
    {
        kv_list_sort(p_bus->chann_array[i].topic_list);
    }
}

static list_node_t* push_message_to_queue(mbus_t* p_bus, mbus_msg_t* p_msg)
{
    /* Allocate memory for message and data */
    list_node_t* p_node = linked_list_alloc_node(&p_bus->msg_queue, sizeof(mbus_msg_t));
    if (p_node == NULL)
    {
        return NULL;
    }

    memcpy(p_node->data_ptr, p_msg, sizeof(mbus_msg_t));

    p_node->opt.value = p_msg->value;

    linked_list_push(&p_bus->msg_queue, p_node);

    return p_node;
}

void mbus_produce_message(mbus_t* p_bus, uint32_t chann, uint32_t topic, uint8_t* p_data, uintptr_t value)
{
    mbus_msg_t msg;

    msg.chann_id = chann;
    msg.topic_id = topic;
    msg.data_ptr = p_data;
    msg.value    = value;

    list_node_t* p_node = push_message_to_queue(p_bus, &msg);
    if (p_node == NULL)
    {
        return;
    }

    hal_event_post(&p_bus->event, MBUS_NEW_EVENT_ID);
}

void mbus_produce_call(mbus_t* p_bus, mbus_topic_fn callback, uint8_t* p_data, uintptr_t value)
{
    mbus_msg_t msg;

    msg.chann_id = MBUS_CALL_GROUP_ID;
    msg.callback = callback;
    msg.data_ptr = p_data;
    msg.value    = value;

    const list_node_t* p_node = push_message_to_queue(p_bus, &msg);
    if (p_node == NULL)
    {
        return;
    }

    hal_event_post(&p_bus->event, MBUS_NEW_EVENT_ID);
}

void mbus_consume_message(mbus_t* p_bus, mbus_msg_t* p_msg)
{
    if (p_msg->chann_id == MBUS_CALL_GROUP_ID)
    {
        /* Perform scheduler call immediately */
        if (p_msg->callback)
        {
            p_msg->callback(p_msg->data_ptr, p_msg->value);
        }
    }
    else
    {
        /* Find topic and perform callback */
        kv_list_t* p_list = p_bus->chann_array[p_msg->chann_id].topic_list;
        kv_node_t* p_node = kv_list_search(p_list, p_msg->topic_id);
        if (p_node)
        {
            mbus_topic_t* p_topic = (mbus_topic_t*)p_node->data_ptr;

            if (p_topic->callback)
            {
                p_topic->callback(p_msg->data_ptr, p_msg->value);
            }
        }
    }
}

void mbus_consume_all(mbus_t* p_bus)
{
    /* Pend the task until receive a signal */
    hal_event_wait(&p_bus->event, MBUS_NEW_EVENT_ID);

    while (1)
    {
        /* Consume all messages in the queue */
        list_node_t* p_node = linked_list_pop(&p_bus->msg_queue);
        if (p_node)
        {
            mbus_consume_message(p_bus, (mbus_msg_t*)p_node->data_ptr);

            linked_list_free_node(&p_bus->msg_queue, p_node);
        }
        else
        {
            break;
        }
    }
}
