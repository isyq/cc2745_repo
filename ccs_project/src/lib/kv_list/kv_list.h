#ifndef KEY_VALUE_LIST_H
#define KEY_VALUE_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_os_api.h"

typedef struct
{
    bool valid;        /* Indicate node is used */
    size_t key;        /* Key of node */
    uint8_t* data_ptr; /* Pointer of data */
} kv_node_t;

typedef struct
{
    size_t count;          /* Used count of node */
    size_t width;          /* Size of node */
    size_t depth;          /* Max count of node */

    kv_node_t* node_array; /* Pointer of node array */
    uint8_t* data_array;   /* Pointer of data array, with the same depth of node_array */

    hal_mutex_t mutex;    /* Mutex for multi-threading */

    bool _sorted;          /* Run `kv_list_sort` will set this flag internally */
} kv_list_t;

#define DEF_STATIC_KV_LIST(name, type, depth)                       \
        static kv_node_t name ## _node_array[(depth)];              \
        static uint8_t name ## _data_array[sizeof(type) * (depth)]; \
        static kv_list_t name = {0, sizeof(type), (depth), name ## _node_array, name ## _data_array};

void kv_list_init(kv_list_t* p_list);
void kv_list_clear(kv_list_t* p_list);
void kv_list_sort(kv_list_t* p_list);
void kv_list_walk(kv_list_t* p_list, size_t* p_key_list, size_t* p_key_count);
void kv_list_remove(kv_list_t* p_list, size_t key);
void kv_list_remove_node(kv_list_t* p_list, kv_node_t* p_node);

kv_node_t* kv_list_add(kv_list_t* p_list, size_t key, void* p_value);
kv_node_t* kv_list_at(kv_list_t* p_list, size_t index);
kv_node_t* kv_list_search(kv_list_t* p_list, size_t key);

#define kv_list_count(p_list) ((p_list)->count)
#define kv_list_empty(p_list) ((p_list)->count == 0)
#define kv_list_full(p_list)  ((p_list)->count == (p_list)->depth)

#endif
