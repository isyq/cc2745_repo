# Lightweight(LW) List

## Design Purpose

Intrusive list implementation.

Typical use case: Linux kernel.

## Design Details

```
    ┌──────────┐  ┌──────────┐  ┌──────────┐      
    │          │  │          │  │          │      
    │  elem    ┼──┤  elem    ┼──┤  elem    │      
    ┼──────────┤  ┼──────────┤  ┼──────────┤      
    │          │  │          │  │          │      
    │          │  │          │  │          │      
    │  data    │  │  data    │  │  data    │      
    │          │  │          │  │          │      
    │          │  │          │  │          │      
    │          │  │          │  │          │      
    └──────────┘  └──────────┘  └──────────┘  

```

## Usage

```c
#define BOOK_INFO_COUNT 10
typedef struct {uint8_t id;char name[32];list_elem_t elem;} book_info_t;

static lw_list_t m_book_list;

static book_info_t m_books[BOOK_INFO_COUNT] = 
{
    {0x01, "book 1"},
    {0x02, "book 2"},
    {0x03, "book 3"},
    {0x04, "book 4"},
    {0x05, "book 5"},
    {0x06, "book 6"},
    {0x07, "book 7"},
    {0x08, "book 8"},
    {0x09, "book 9"},
    {0x0A, "book 10"},
};

DEF_STATIC_LW_LIST_FIND_FUNC(find_book_info, book_info_t, uint8_t, id)

lw_list_init(&m_book_list);

uint8_t book_count = lw_list_count(&m_book_list);
TEST_ASSERT_EQUAL(0, book_count);

for (size_t i = 0; i < BOOK_INFO_COUNT; i++)
{
    lw_list_push(&m_book_list, &m_books[i].elem);
}

book_count = lw_list_count(&m_book_list);
TEST_ASSERT_EQUAL(BOOK_INFO_COUNT, book_count);

lw_list_pop(&m_book_list);

book_count = lw_list_count(&m_book_list);
TEST_ASSERT_EQUAL(BOOK_INFO_COUNT - 1, book_count);

list_elem_t* p_elem = find_book_info(&m_book_list, 0x05);
TEST_ASSERT_NOT_NULL(p_elem);
book_info_t* p_book = lw_list_node(p_elem, book_info_t, elem);
TEST_ASSERT_EQUAL(0x05, p_book->id);
TEST_ASSERT_EQUAL_STRING("book 5", p_book->name);

```

### lw_list vs linked_list 

https://www.zhihu.com/question/400354490
