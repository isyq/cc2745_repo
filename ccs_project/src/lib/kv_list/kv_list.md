## KV List

### Purpose:

  - Lookup table. Add items and sort by key, then use binary search to find the item quickly
  - Dictionary structure. Such as BLE link, each link has an unique handle as key.

### Usage:

```C
typedef struct
{
    uint8_t id;
    char name[16];
} book_info_t;

DEF_STATIC_KV_PARAM(m_kv_param, book_info_t, 100);

static kv_list_t m_kv_list;
kv_list_init(&m_kv_list, &m_kv_param);

book_info_t book1 = {1, "book1"};
book_info_t book2 = {2, "book2"};
book_info_t book3 = {3, "book3"};

size_t book_count = kv_list_count(&m_kv_list);
TEST_ASSERT_EQUAL(0, book_count);

kv_node_t* p_node = kv_list_add(&m_kv_list, book1.id);
kv_list_set_node_data(p_node, &book1, sizeof(book1));

p_node = kv_list_add(&m_kv_list, book3.id);
kv_list_set_node_data(p_node, &book3, sizeof(book3));

p_node = kv_list_add(&m_kv_list, book2.id);
kv_list_set_node_data(p_node, &book2, sizeof(book2));

book_count = kv_list_count(&m_kv_list);
TEST_ASSERT_EQUAL(3, book_count);

p_node = kv_node_ptr_at(&m_kv_list, 2);
TEST_ASSERT_EQUAL_PTR(book2.id, p_node->key);

p_node = kv_node_ptr_at(&m_kv_list, 1);
TEST_ASSERT_EQUAL_PTR(book3.id, p_node->key);

size_t key_array[3];
kv_list_walk(&m_kv_list, key_array, NULL);

TEST_ASSERT_EQUAL(1, key_array[0]);
TEST_ASSERT_EQUAL(3, key_array[1]);
TEST_ASSERT_EQUAL(2, key_array[2]);

kv_list_sort(&m_kv_list);

kv_list_walk(&m_kv_list, key_array, NULL);

TEST_ASSERT_EQUAL(1, key_array[0]);
TEST_ASSERT_EQUAL(2, key_array[1]);
TEST_ASSERT_EQUAL(3, key_array[2]);
```

### FAQ

  - Support heap based structure?
    - No.
    - If support heap, need to add list node member, which makes thing complicated.

  