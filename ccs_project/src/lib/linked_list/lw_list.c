#include "lw_list.h"

void lw_list_init(lw_list_t* p_list)
{
    p_list->head  = NULL;
    p_list->tail  = NULL;
    p_list->count = 0;

    hal_mutex_init(&p_list->mutex);
}

list_elem_t* lw_list_insert_head(lw_list_t* p_list, list_elem_t* p_elem)
{
    hal_mutex_lock(&p_list->mutex);

    LIST_INSERT_HEAD(p_list, p_elem);

    p_list->count++;

    hal_mutex_unlock(&p_list->mutex);

    return p_elem;
}

list_elem_t* lw_list_insert_tail(lw_list_t* p_list, list_elem_t* p_elem)
{
    hal_mutex_lock(&p_list->mutex);

    LIST_INSERT_TAIL(p_list, p_elem);

    p_list->count++;

    hal_mutex_unlock(&p_list->mutex);

    return p_elem;
}

list_elem_t* lw_list_remove_head(lw_list_t* p_list)
{
    hal_mutex_lock(&p_list->mutex);

    list_elem_t* p_head = p_list->head;

    /* If p_head is non-NULL, it's a non-empty list */
    if (p_head)
    {
        LIST_REMOVE_HEAD(p_list, p_head);

        p_list->count--;
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_head;
}

list_elem_t* lw_list_remove_tail(lw_list_t* p_list)
{
    hal_mutex_lock(&p_list->mutex);

    list_elem_t* p_tail = p_list->tail;

    /* If p_tail is non-NULL, it's a non-empty list */
    if (p_tail)
    {
        LIST_REMOVE_TAIL(p_list, p_tail);

        p_list->count--;
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_tail;
}

list_elem_t* lw_list_remove_node(lw_list_t* p_list, list_elem_t* p_elem)
{
    hal_mutex_lock(&p_list->mutex);

    list_elem_t* p_out_node = p_elem;

    if (p_elem)
    {
        LIST_REMOVE_NODE(p_list, p_elem);

        p_list->count--;
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_out_node;
}

bool lw_list_exist(lw_list_t* p_list, list_elem_t* p_elem)
{
    bool found = false;

    LIST_NODE_EXISTS(p_list, p_elem, &found);

    return found;
}

size_t lw_list_count(lw_list_t* p_list)
{
    return p_list->count;
}
