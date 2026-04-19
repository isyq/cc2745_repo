#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "ret_code.h"

#define STTM_STATE_ANY  0xFF
#define STTM_STATE_NONE 0xFE

typedef uint8_t sttm_id_t;
typedef void (*sttm_action_fn)(sttm_id_t event);

typedef struct
{
    sttm_id_t id;
    sttm_action_fn enter;
    sttm_action_fn exit;
} sttm_state_t;

typedef struct
{
    sttm_id_t id;
    sttm_id_t source;
    sttm_id_t target;
} sttm_event_t;

typedef void (*sttm_callback_fn)(sttm_state_t* source, sttm_state_t* target);

typedef struct
{
    sttm_state_t* state_array;
    sttm_event_t* event_array;
    uint8_t state_count;
    uint8_t event_count;
    sttm_state_t* current;
    sttm_callback_fn callback;
} sttm_machine_t;

#define DEF_STATIC_STTM_MACHINE(name, state_array, state_count, event_array, event_count) \
    static sttm_machine_t name = {state_array, event_array, state_count, event_count, NULL, NULL};

void sttm_init(sttm_machine_t* p_machine, sttm_state_t* p_init_state, sttm_callback_fn callback);
void sttm_set_state(sttm_machine_t* p_machine, sttm_id_t state_id);
sttm_state_t* sttm_get_state(sttm_machine_t* p_machine);
void sttm_transit(sttm_machine_t* p_machine, sttm_id_t event_id);

#endif
