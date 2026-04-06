# Linked List

## Design Purpose

- Support array based list and heap based list, and they use the same interface.
- Support most opertions of list: Add/Remove/Find/Sort

## Design Details


Array based list:

```
    Bind node array to data array:

            ┌───┬───┬───┬───┬───┐
    node    │ 1 │ 0 │ 1 │ 0 │ 1 │   used=1: allocated, used=0: free
    array   └─┬─┴─┬─┴─┬─┴─┬─┴──┬┘
              │   │   │   │    └──────────────────────────────┐
              │   │   │   └───────────────────────┐           │
              │   │   └───────────────┐           │           │
              │   └───────┐           │           │           │
    data    ┌─▼─────────┬─▼─────────┬─▼─────────┬─▼─────────┬─▼─────────┐
    array   │  xxxx     │           │  xxxx     │           │  xxxx     │
            └───────────┴───────────┴───────────┴───────────┴───────────┘
```

Heap based list:

```
     ┌───p_node                               
     ▼                                        
     ┌───────────────┬──────────────────────┐ 
     │      node     │         data         │ 
     └───────────────└──────────────────────┘ 
                     ▲                        
                     │                        
      p_node->data_ptr      
```

## Usage

1. Array based list

```C
#define BOOK_INFO_COUNT 10
typedef struct {uint8_t id; char name[32];} book_info_t;

DEF_STATIC_LIST_PARAM(m_list_param, book_info_t, BOOK_INFO_COUNT)
static linked_list_t m_book_list;

linked_list_init(&m_book_list, &m_list_param);

list_node_t* p_node = linked_list_alloc_node(&m_book_list, sizeof(book_info_t));

book_info_t test_book = {1, "test book"};
linked_list_set_node_data(p_node, &test_book, sizeof(book_info_t));

linked_list_insert_tail(&m_book_list, p_node);

book_count = linked_list_count(&m_book_list);

```

2. Heap based list

```C
linked_list_t book_list;
linked_list_init(&book_list, NULL);

book_info_t m_test_data[] =
{
    {0x03}, {0x01}, {0x04}, {0x00}, {0x05}, {0x09}, {0x02}, {0x06},
};

for (uint8_t i = 0; i < sizeof(m_test_data) / sizeof(m_test_data[0]); i++)
{
    list_node_t* p_node = linked_list_alloc_node(&book_list, sizeof(book_info_t));
    if (p_node)
    {
        *(book_info_t*)p_node->data_ptr = m_test_data[i];
        linked_list_insert_tail(&book_list, p_node);
    }
}

LINKED_LIST_FOR_EACH(&book_list, p_node)
{
    book_info_t* p_data = (book_info_t*)p_node->data_ptr;

    printf("Before id: %x", p_data->id);
}

```
