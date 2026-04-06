#include "linked_list.h"
#include "util_tool.h"

static const size_t m_aligned_node_len = ALIGN_UP(sizeof(list_node_t), ALIGN_SIZE);

void linked_list_init_for_array(linked_list_t* p_list)
{
    p_list->head = NULL;
    p_list->tail = NULL;

    p_list->count   = 0;
    p_list->_sorted = false;

    if (p_list->_array_based)
    {
        if (p_list->depth == 0 || p_list->width == 0 ||
            p_list->node_array == NULL || p_list->data_array == NULL)
        {
            return;
        }

        for (size_t i = 0; i < p_list->depth; i++)
        {
            list_node_t* p_node = &p_list->node_array[i];

            p_node->next = NULL;
            p_node->prev = NULL;

            p_node->opt.used = false;

            /**
             * Bind node array to data array:
             *
             *          ┌───┬───┬───┬───┬───┐
             *  node    │ 1 │ 0 │ 1 │ 0 │ 1 │   used=1: allocated, used=0: free
             *  array   └─┬─┴─┬─┴─┬─┴─┬─┴──┬┘
             *            │   │   │   │    └──────────────────────────────┐
             *            │   │   │   └───────────────────────┐           │
             *            │   │   └───────────────┐           │           │
             *            │   └───────┐           │           │           │
             *  data    ┌─▼─────────┬─▼─────────┬─▼─────────┬─▼─────────┬─▼─────────┐
             *  array   │  xxxx     │           │  xxxx     │           │  xxxx     │
             *          └───────────┴───────────┴───────────┴───────────┴───────────┘
             *
             */
            p_node->data_ptr = &p_list->data_array[i * p_list->width];
        }

        /* Clear data of data_array */
        memset(p_list->data_array, 0, p_list->depth * p_list->width);

        p_list->_last_slot_index = 0;
    }

    hal_mutex_init(&p_list->mutex);
}

void linked_list_init_for_heap(linked_list_t* p_list)
{
    p_list->head = NULL;
    p_list->tail = NULL;

    p_list->count   = 0;
    p_list->_sorted = false;

    hal_mutex_init(&p_list->mutex);
}

void linked_list_free_node(linked_list_t* p_list, list_node_t* p_node)
{
    p_node->next = NULL;
    p_node->prev = NULL;

    if (p_list->_array_based)
    {
        /* Keep p_node->data_ptr of array list after initialization */
        p_node->opt.used = false;
    }
    else
    {
        p_node->opt.value = 0;

        /* Free both node and node.data_ptr */
        OSAL_FREE(p_node);
    }
}

static list_node_t* alloc_array_node(linked_list_t* p_list)
{
    /* Record last index to speed up searching */
    size_t last_slot_index = p_list->_last_slot_index;

    list_node_t* p_node = NULL;
    for (size_t i = 0; i < p_list->depth; i++)
    {
        size_t index;

        /* If depth value is power of 2, use fast method to get mod */
        bool is_power2 = POWER2_CHECK(p_list->depth);
        if (is_power2)
        {
            index = POWER2_GET_MOD(last_slot_index + i, p_list->depth);
        }
        else
        {
            index = (last_slot_index + i) % p_list->depth;
        }

        list_node_t* p_iter = &p_list->node_array[index];

        /* Find the first free node */
        if (!p_iter->opt.used)
        {
            p_iter->opt.used = true;

            p_node = p_iter;

            p_list->_last_slot_index = index;
            break;
        }
    }

    return p_node;
}

static list_node_t* alloc_heap_node(size_t data_len)
{
    /* Set memory alignment */
    size_t aligned_data_len = ALIGN_UP(data_len, ALIGN_SIZE);

    /* Allocate memory for node and data */
    list_node_t* p_node = (list_node_t*)OSAL_MALLOC(m_aligned_node_len + aligned_data_len);
    if (p_node)
    {
        memset(p_node, 0, sizeof(m_aligned_node_len + aligned_data_len));
        /* data_ptr: add an offset of p_node */
        p_node->data_ptr = (uint8_t*)p_node + m_aligned_node_len;
    }

    return p_node;
}

list_node_t* linked_list_alloc_node(linked_list_t* p_list, size_t data_len)
{
    hal_mutex_lock(&p_list->mutex);

    list_node_t* p_node = NULL;

    if (p_list->_array_based)
    {
        p_node = alloc_array_node(p_list);
    }
    else
    {
        p_node = alloc_heap_node(data_len);
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_node;
}

list_node_t* linked_list_insert_head(linked_list_t* p_list, list_node_t* p_node)
{
    hal_mutex_lock(&p_list->mutex);

    LIST_INSERT_HEAD(p_list, p_node);

    p_list->count++;
    p_list->_sorted = false;

    hal_mutex_unlock(&p_list->mutex);

    return p_node;
}

list_node_t* linked_list_insert_tail(linked_list_t* p_list, list_node_t* p_node)
{
    hal_mutex_lock(&p_list->mutex);

    LIST_INSERT_TAIL(p_list, p_node);

    p_list->count++;
    p_list->_sorted = false;

    hal_mutex_unlock(&p_list->mutex);

    return p_node;
}

list_node_t* linked_list_remove_head(linked_list_t* p_list)
{
    hal_mutex_lock(&p_list->mutex);

    list_node_t* p_head = p_list->head;

    /* If p_head is non-NULL, it's a non-empty list */
    if (p_head)
    {
        LIST_REMOVE_HEAD(p_list, p_head);

        p_list->count--;
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_head;
}

list_node_t* linked_list_remove_tail(linked_list_t* p_list)
{
    hal_mutex_lock(&p_list->mutex);

    list_node_t* p_tail = p_list->tail;

    /* If p_head is non-NULL, it's a non-empty list */
    if (p_tail)
    {
        LIST_REMOVE_TAIL(p_list, p_tail);

        p_list->count--;
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_tail;
}

list_node_t* linked_list_remove_node(linked_list_t* p_list, list_node_t* p_node)
{
    hal_mutex_lock(&p_list->mutex);

    list_node_t* p_out_node = p_node;

    if (p_node)
    {
        LIST_REMOVE_NODE(p_list, p_node);

        p_list->count--;
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_out_node;
}

bool linked_list_exist(linked_list_t* p_list, list_node_t* p_node)
{
    hal_mutex_lock(&p_list->mutex);

    bool found = false;

    LIST_NODE_EXISTS(p_list, p_node, &found);

    hal_mutex_unlock(&p_list->mutex);

    return found;
}

size_t linked_list_count(linked_list_t* p_list)
{
    hal_mutex_lock(&p_list->mutex);

    size_t count = p_list->count;

    hal_mutex_unlock(&p_list->mutex);

    return count;
}

list_node_t* linked_list_search(linked_list_t* p_list, const void* p_data, list_cmp_fn compare)
{
    hal_mutex_lock(&p_list->mutex);

    list_node_t* p_node = NULL;

    list_node_t target_node = {.opt.used = true, .data_ptr = (uint8_t*)p_data};

    if (p_list->_sorted && p_list->_array_based)
    {
        /* If list have _sorted, use binary search */
        p_node = bsearch(&target_node, p_list->node_array, p_list->depth, sizeof(list_node_t), compare);
    }
    else
    {
        LINKED_LIST_FOR_EACH(p_list, p_iter_node)
        {
            if (compare(p_iter_node, &target_node) == 0)
            {
                p_node = p_iter_node;

                break;
            }
        }
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_node;
}

static void qsort_array_list(linked_list_t* p_list, list_cmp_fn compare)
{
    hal_mutex_lock(&p_list->mutex);

    size_t count = p_list->count;

    if (count == 0)
    {
        hal_mutex_unlock(&p_list->mutex);

        return;
    }

    /* Always process whole array, so the 2nd param is depth */
    qsort(p_list->node_array, p_list->depth, sizeof(list_node_t), compare);

    /* Rebuild node arms */
    for (size_t i = 0; i < count - 1; i++)
    {
        p_list->node_array[i].next     = &p_list->node_array[i + 1];
        p_list->node_array[i + 1].prev = &p_list->node_array[i];
    }

    p_list->head = &p_list->node_array[0];
    p_list->tail = &p_list->node_array[count - 1];

    p_list->head->prev = NULL;
    p_list->tail->next = NULL;

    hal_mutex_unlock(&p_list->mutex);
}

static void qsort_heap_list(linked_list_t* p_list, list_cmp_fn compare)
{
    hal_mutex_lock(&p_list->mutex);

    size_t count = p_list->count;

    if (count == 0)
    {
        hal_mutex_unlock(&p_list->mutex);

        return;
    }

    // TODO: implement it

    hal_mutex_unlock(&p_list->mutex);
}

void linked_list_sort(linked_list_t* p_list, list_cmp_fn compare)
{
    hal_mutex_lock(&p_list->mutex);

    if (p_list->_array_based)
    {
        qsort_array_list(p_list, compare);
    }
    else
    {
        qsort_heap_list(p_list, compare);
    }

    p_list->_sorted = true;

    hal_mutex_unlock(&p_list->mutex);
}
