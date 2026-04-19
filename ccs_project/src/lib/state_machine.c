#include "state_machine.h"

void sttm_init(sttm_machine_t* p_machine, sttm_state_t* p_init_state, sttm_callback_fn callback)
{
    p_machine->current  = p_init_state;
    p_machine->callback = callback;
}

static sttm_state_t* find_state(sttm_machine_t* p_machine, sttm_id_t state_id)
{
    sttm_state_t* p_out_state = NULL;

    for (uint8_t i = 0; i < p_machine->state_count; i++)
    {
        if (p_machine->state_array[i].id == state_id)
        {
            p_out_state = &p_machine->state_array[i];
            break;
        }
    }

    return p_out_state;
}

static sttm_event_t* find_event(sttm_machine_t* p_machine, sttm_id_t event_id)
{
    sttm_event_t* p_out_event = NULL;

    for (uint8_t i = 0; i < p_machine->event_count; i++)
    {
        if (p_machine->event_array[i].id == event_id)
        {
            p_out_event = &p_machine->event_array[i];
            break;
        }
    }

    return p_out_event;
}

void sttm_set_state(sttm_machine_t* p_machine, sttm_id_t state_id)
{
    p_machine->current = find_state(p_machine, state_id);
}

sttm_state_t* sttm_get_state(sttm_machine_t* p_machine)
{
    return p_machine->current;
}

void sttm_transit(sttm_machine_t* p_machine, sttm_id_t event_id)
{
    sttm_event_t* p_event = find_event(p_machine, event_id);
    if (p_event ==  NULL)
    {
        return;
    }

    if (p_machine->current->id != p_event->source)
    {
        return;
    }

    sttm_state_t* p_target_state = find_state(p_machine, p_event->target);
    p_machine->callback(p_machine->current, p_target_state);
}
