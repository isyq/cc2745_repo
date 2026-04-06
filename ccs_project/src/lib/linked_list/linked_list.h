#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "list_algo_macro.h"
#include "hal_os_api.h"

struct _list_node
{
    struct _list_node* next; /* Pointer of next node */
    struct _list_node* prev; /* Pointer of previous node */
    union
    {
        bool used;           /* In array list, it indicates whether item is allocated. */
        uintptr_t value;     /* In heap list, it holds a user data(value or pointer). */
    } opt;
    uint8_t* data_ptr;       /* Pointer of item address of data_array */
};

typedef struct _list_node list_node_t;

typedef struct
{
    list_node_t* head;       /* Pointer of head node */
    list_node_t* tail;       /* Pointer of tail node */

    bool _array_based;       /* true: array list, false: heap list */

    size_t count;            /* Used count of nodes */
    size_t width;            /* Byte length of node */
    size_t depth;            /* Max count of node */

    list_node_t* node_array; /* Node array pointer */
    uint8_t* data_array;     /* Data array pointer, with the same depth of node_array */

    hal_mutex_t mutex;       /* Mutex for multi-threading */

    bool _sorted;            /* List is sorted */
    size_t _last_slot_index; /* Index of last allocated node slot, for internal use only */
} linked_list_t;

void linked_list_init_for_array(linked_list_t* p_list);
void linked_list_init_for_heap(linked_list_t* p_list);
void linked_list_free_node(linked_list_t* p_list, list_node_t* p_node);

list_node_t* linked_list_alloc_node(linked_list_t* p_list, size_t data_len);
list_node_t* linked_list_insert_head(linked_list_t* p_list, list_node_t* p_node);
list_node_t* linked_list_insert_tail(linked_list_t* p_list, list_node_t* p_node);
list_node_t* linked_list_remove_head(linked_list_t* p_list);
list_node_t* linked_list_remove_tail(linked_list_t* p_list);
list_node_t* linked_list_remove_node(linked_list_t* p_list, list_node_t* p_node);

bool linked_list_exist(linked_list_t* p_list, list_node_t* p_node);
size_t linked_list_count(linked_list_t* p_list);

/**
 * Ref: https://en.cppreference.com/w/c/algorithm/qsort
 *
 * Eample:
 *
 * typedef struct { uint8_t id; } my_data_t;
 * static int compare_num(const void* p_node_1, const void* p_node_2)
 * {
 *    const list_node_t* p_node1 = (const list_node_t*)p_node_1;
 *    const list_node_t* p_node2 = (const list_node_t*)p_node_2;
 *
 *    if (likely(p_node1->opt.used && p_node2->opt.used))
 *    {
 *        const my_data_t* p_data_1 = (const my_data_t*)p_node1->data_ptr;
 *        const my_data_t* p_data_2 = (const my_data_t*)p_node2->data_ptr;
 *
 *        return (p_data_1->id > p_data_2->id) - (p_data_1->id < p_data_2->id);
 *    }
 *    else
 *    {
 *        return p_node2->opt.used - p_node1->opt.used;
 *    }
 * }
 */
typedef int (*list_cmp_fn)(const void* p_node_1, const void* p_node_2);

list_node_t* linked_list_search(linked_list_t* p_list, const void* p_data, list_cmp_fn compare);
void linked_list_sort(linked_list_t* p_list, list_cmp_fn compare);

#define linked_list_push linked_list_insert_tail
#define linked_list_pop  linked_list_remove_head

#define linked_list_peek(p_list)  ((p_list)->head)
#define linked_list_empty(p_list) ((p_list)->count == 0)

#define linked_list_set_node_data(p_node, p_data, data_len) memcpy((p_node)->data_ptr, (p_data), (data_len))
#define linked_list_get_node_data(p_node)                   ((p_node)->data_ptr)

#define DEF_STATIC_ARRAY_LIST(name, type, depth)                     \
        static list_node_t name ## _node_array[(depth)];             \
        static uint8_t* name ## _data_array[sizeof(type) * (depth)]; \
        static linked_list_t name = {0, 0, 1, 0, sizeof(type), (depth), name ## _node_array, name ## _data_array};

#define DEF_STATIC_HEAP_LIST(name, type, depth) \
        static linked_list_t name = {0, 0, 0};

#define LINKED_LIST_FOR_EACH(p_list, p_node) \
        for (list_node_t* (p_node) = (p_list)->head; (p_node) != NULL; (p_node) = (p_node)->next)

#define DEF_STATIC_LIST_FIND_FUNC(fn_name, node_type, item_type, item_name)         \
        static list_node_t* fn_name(linked_list_t * p_list, item_type target_value) \
        {                                                                           \
            list_node_t* p_out_node = NULL;                                         \
            list_node_t* p_node;                                                    \
            for (p_node = (p_list)->head; p_node != NULL; p_node = p_node->next)    \
            {                                                                       \
                if (((node_type*)(p_node->data_ptr))->item_name == (target_value))  \
                {                                                                   \
                    p_out_node = p_node;                                            \
                    break;                                                          \
                }                                                                   \
            }                                                                       \
            return p_out_node;                                                      \
        }

#endif
