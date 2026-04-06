#include <stdlib.h>
#include <string.h>
#include "kv_list.h"

static const size_t KV_KEY_INVALID = (size_t)(-1);

void kv_list_init(kv_list_t* p_list)
{
    p_list->count      = 0;
    p_list->_sorted    = false;

    /* Bind node array to data array by index */
    for (size_t i = 0; i < p_list->depth; i++)
    {
        kv_node_t* p_node = &p_list->node_array[i];

        p_node->data_ptr = &p_list->data_array[i * p_list->width];
        p_node->key      = KV_KEY_INVALID;
    }

    hal_mutex_init(&p_list->mutex);
}

kv_node_t* kv_list_add(kv_list_t* p_list, size_t key, void* p_value)
{
    if (p_list->count >= p_list->depth)
    {
        return NULL;
    }

    hal_mutex_lock(&p_list->mutex);

    kv_node_t* p_node = NULL;

    for (size_t i = 0; i < p_list->depth; i++)
    {
        if (!p_list->node_array[i].valid)
        {
            p_list->node_array[i].valid = true;
            p_list->node_array[i].key   = key;

            memcpy(p_list->node_array[i].data_ptr, p_value, p_list->width);

            p_list->count++;
            p_list->_sorted = false;

            p_node = &p_list->node_array[i];
            break;
        }
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_node;
}

void kv_list_clear(kv_list_t* p_list)
{
    hal_mutex_lock(&p_list->mutex);

    for (size_t i = 0; i < p_list->depth; i++)
    {
        p_list->node_array[i].valid = false;
        p_list->node_array[i].key = KV_KEY_INVALID;
    }
    
    p_list->count = 0;
    p_list->_sorted = false;
    
    hal_mutex_unlock(&p_list->mutex);
}

kv_node_t* kv_list_at(kv_list_t* p_list, size_t index)
{
    if (index >= p_list->count)
    {
        return NULL;
    }

    hal_mutex_lock(&p_list->mutex);

    size_t i = 0;
    kv_node_t* p_node = NULL;

    while (i < p_list->count)
    {
        if (p_list->node_array[index].valid)
        {
            i++;
        }

        if (i == index)
        {
            p_node = &p_list->node_array[index];
            break;
        }
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_node;
}

int compare_node_key(const void* p_node_1, const void* p_node_2)
{
    /* Ref: https://en.cppreference.com/w/c/algorithm/qsort */
    const kv_node_t* p_node1 = (const kv_node_t*)p_node_1;
    const kv_node_t* p_node2 = (const kv_node_t*)p_node_2;

    if (!p_node1->valid && !p_node2->valid)
    {
        return 0;
    }
    else if (p_node1->valid && p_node2->valid)
    {
        return (p_node1->key > p_node2->key) - (p_node1->key < p_node2->key);
    }
    else
    {
        return p_node2->valid - p_node1->valid;
    }
}

void kv_list_sort(kv_list_t* p_list)
{
    hal_mutex_lock(&p_list->mutex);

    qsort(p_list->node_array, p_list->depth, sizeof(kv_node_t), compare_node_key);

    p_list->_sorted = true;

    hal_mutex_unlock(&p_list->mutex);
}

static kv_node_t* general_search(kv_list_t* p_list, size_t key)
{
    kv_node_t* p_node = NULL;

    for (size_t i = 0, j = 0; i < p_list->depth && j < p_list->count; i++)
    {
        if (p_list->node_array[i].valid)
        {
            j++;
            if (p_list->node_array[i].key == key)
            {
                p_node = &p_list->node_array[i];

                break;
            }
        }
    }

    return p_node;
}

static kv_node_t* binary_search(kv_list_t* p_list, size_t key)
{
    kv_node_t target_node = {true, key, NULL};

    return (kv_node_t*)bsearch(&target_node, p_list->node_array, p_list->depth, sizeof(kv_node_t), compare_node_key);
}

kv_node_t* kv_list_search(kv_list_t* p_list, size_t key)
{
    hal_mutex_lock(&p_list->mutex);

    kv_node_t* p_node = NULL;

    if (p_list->_sorted)
    {
        p_node = binary_search(p_list, key);
    }
    else
    {
        p_node = general_search(p_list, key);
    }

    hal_mutex_unlock(&p_list->mutex);

    return p_node;
}

void kv_list_walk(kv_list_t* p_list, size_t* p_key_array, size_t* p_key_count)
{
    hal_mutex_lock(&p_list->mutex);

    size_t count = 0;
    for (size_t i = 0; i < p_list->depth; i++)
    {
        if (p_list->node_array[i].valid)
        {
            p_key_array[count] = p_list->node_array[i].key;
            count++;
        }
    }

    if (p_key_count)
    {
        *p_key_count = count;
    }

    hal_mutex_unlock(&p_list->mutex);
}

void kv_list_remove_node(kv_list_t* p_list, kv_node_t* p_node)
{
    if (!p_node->valid)
    {
        return;
    }

    hal_mutex_lock(&p_list->mutex);

    p_node->valid = false;
    p_node->key   = KV_KEY_INVALID;

    p_list->count--;
    p_list->_sorted = false;

    hal_mutex_unlock(&p_list->mutex);
}

void kv_list_remove(kv_list_t* p_list, size_t key)
{
    kv_node_t* p_node = kv_list_search(p_list, key);
    kv_list_remove_node(p_list, p_node);
}

