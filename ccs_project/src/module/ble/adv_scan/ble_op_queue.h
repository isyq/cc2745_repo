#ifndef BLE_OP_H
#define BLE_OP_H

#include "bit_state.h"
#include "linked_list.h"
#include "ret_code.h"

typedef ret_t (*ble_op_runner_fn)(uintptr_t param);

typedef struct
{
    char* name;
    uint8_t id;
    bit_state_t target_state;
    bit_state_t precondition;
    uintptr_t param;
    ble_op_runner_fn runner_callback;
} ble_op_t;

typedef void (*ble_op_result_fn)(ble_op_t* p_op, ret_t ret);

typedef struct
{
    linked_list_t op_list;
    ble_op_result_fn result_callback;
    bit_state_t current_state;
    ble_op_t* running_op_ptr;
} ble_op_queue_t;

void ble_opq_init(ble_op_queue_t* p_queue, bit_state_t curr_state, ble_op_result_fn result_callback);
void ble_opq_process(ble_op_queue_t* p_queue);
void ble_opq_start(ble_op_queue_t* p_queue, ble_op_t* p_op);
void ble_opq_complete(ble_op_queue_t* p_queue, ble_op_t* p_op);
void ble_opq_set_state(ble_op_queue_t* p_queue, bit_state_t state);
bit_state_t ble_opq_get_state(ble_op_queue_t* p_queue);

#endif
