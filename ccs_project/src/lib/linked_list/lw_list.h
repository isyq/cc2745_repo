#ifndef LIGHT_WEIGHT_LIST_H
#define LIGHT_WEIGHT_LIST_H

#include "list_algo_macro.h"
#include "hal_os_api.h"
#include "util_tool.h"

struct _list_elem
{
    struct _list_elem* next; /* Pointer of next node */
    struct _list_elem* prev; /* Pointer of previous node */
};

typedef struct _list_elem list_elem_t;

typedef struct
{
    list_elem_t* head;  /* Pointer of head node */
    list_elem_t* tail;  /* Pointer of tail node */
    hal_mutex_t mutex; /* Mutex for multi-threading */
    size_t count;       /* Used count of nodes */
} lw_list_t;

void lw_list_init(lw_list_t* p_list);

list_elem_t* lw_list_insert_head(lw_list_t* p_list, list_elem_t* p_elem);
list_elem_t* lw_list_insert_tail(lw_list_t* p_list, list_elem_t* p_elem);
list_elem_t* lw_list_remove_head(lw_list_t* p_list);
list_elem_t* lw_list_remove_tail(lw_list_t* p_list);
list_elem_t* lw_list_remove_node(lw_list_t* p_list, list_elem_t* p_elem);

bool lw_list_exist(lw_list_t* p_list, list_elem_t* p_elem);

size_t lw_list_count(lw_list_t* p_list);

#define lw_list_push lw_list_insert_tail
#define lw_list_pop  lw_list_remove_head

#define lw_list_peek(p_list)  ((p_list)->head)
#define lw_list_empty(p_list) ((p_list)->head == NULL)

/**
 * Example:
 *
 * typedef struct {list_elem_t elem; int id; char* name;} book_info_t;
 * list_elem_t* p_elem = lw_list_find(p_list, my_book);
 * book_info_t* p_book = lw_list_node(p_elem, book_info_t, elem);
 */
#define lw_list_node(p_elem, node_type, elem_name) container_of(p_elem, node_type, elem_name)
#define lw_list_elem(p_node)                       (&(p_node->elem))

#define LW_LIST_FOR_EACH(p_list, p_elem) \
        for (list_elem_t * (p_elem) = (p_list)->head; (p_elem) != NULL; (p_elem) = (p_elem)->next)

#define DEF_STATIC_LW_LIST_FIND_FUNC(fn_name, node_type, item_type, item_name)   \
        static list_elem_t* fn_name(lw_list_t * p_list, item_type target_value)  \
        {                                                                        \
            list_elem_t* p_out_node = NULL;                                      \
            list_elem_t* p_node;                                                 \
            for (p_node = (p_list)->head; p_node != NULL; p_node = p_node->next) \
            {                                                                    \
                node_type* p_data = container_of(p_node, node_type, elem);       \
                if (p_data->item_name == (target_value))                         \
                {                                                                \
                    p_out_node = p_node;                                         \
                    break;                                                       \
                }                                                                \
            }                                                                    \
            return p_out_node;                                                   \
        }

#endif
