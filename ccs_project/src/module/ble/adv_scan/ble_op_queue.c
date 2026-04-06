#include "ble_op_queue.h"
#include "log.h"

void ble_opq_init(ble_op_queue_t* p_queue, bit_state_t current_state, ble_op_result_fn result_callback)
{
    p_queue->current_state   = current_state;
    p_queue->running_op_ptr  = NULL;
    p_queue->result_callback = result_callback;

    linked_list_init_for_heap(&p_queue->op_list);
}

static bool perform_command(ble_op_queue_t* p_queue, ble_op_t* p_op)
{
    ret_t ret = RET_OK;

    bool wait_event = false;

    /* If current is in target state, do nothing */
    bool target_state_matched = bit_state_match(p_queue->current_state, p_op->target_state);
    if (!target_state_matched)
    {
        /* Check precondition is matched */
        bool precondition_state_matched = bit_state_match(p_queue->current_state, p_op->precondition);
        if (precondition_state_matched)
        {
            /* If precondition state matches, perform the operation */
            log_info("OP start: %s", p_op->name);

            if (p_op->runner_callback != NULL)
            {
                ret = p_op->runner_callback(p_op->param);
                if (ret == RET_OK)
                {
                    p_queue->running_op_ptr = p_op;

                    wait_event = true;
                }
            }
            else
            {
                /* If runner_callback is NULL, enter target state directly */
                p_queue->current_state = p_op->target_state;
            }
        }
        else
        {
            /* If another operation is ongoing, wait its event */
            if (p_queue->running_op_ptr != NULL)
            {
                wait_event = true;
            }

            ret = RET_ERR_STATE_NOT_MATCH;
        }
    }
    else
    {
        log_debug("OP state matched: %d, %d", p_queue->current_state, p_op->target_state);
    }

    return wait_event;
}

void ble_opq_process(ble_op_queue_t* p_queue)
{
    ret_t ret = RET_OK;

    list_node_t* p_node;
    while (true)
    {
        p_node = linked_list_pop(&p_queue->op_list);
        if (p_node == NULL)
        {
            break;
        }

        ble_op_t* p_op = (ble_op_t*)p_node->data_ptr;

        bool wait_event = perform_command(p_queue, p_op);
        if (wait_event)
        {
            /* If need to wait event, stop handling more operations */
            break;
        }
    }
}

void ble_opq_start(ble_op_queue_t* p_queue, ble_op_t* p_op)
{
    bool is_empty = linked_list_empty(&p_queue->op_list);
    if (p_queue->running_op_ptr == NULL && is_empty)
    {
        /* If there is no pending operation, perform it immediately */
        perform_command(p_queue, p_op);
    }
    else
    {
        /* Otherwise, push it to the queue */
        list_node_t* p_node = linked_list_alloc_node(&p_queue->op_list, sizeof(ble_op_t));
        linked_list_set_node_data(p_node, p_op, sizeof(ble_op_t));
        linked_list_push(&p_queue->op_list, p_node);
    }
}

void ble_opq_complete(ble_op_queue_t* p_queue, ble_op_t* p_op)
{
    log_info("OP finish: %s", p_op->name);

    p_queue->current_state  = p_op->target_state;
    p_queue->running_op_ptr = NULL;
}

void ble_opq_set_state(ble_op_queue_t* p_queue, bit_state_t state)
{
    p_queue->current_state = state;
}

bit_state_t ble_opq_get_state(ble_op_queue_t* p_queue)
{
    return p_queue->current_state;
}
