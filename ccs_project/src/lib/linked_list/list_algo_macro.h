#ifndef LIST_ALGO_MACRO_H
#define LIST_ALGO_MACRO_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdatomic.h>

#define LIST_INSERT_HEAD(p_list, p_node)         \
        do {                                     \
            if ((p_list)->head == NULL)          \
            {                                    \
                (p_node)->prev = NULL;           \
                (p_node)->next = NULL;           \
                                                 \
                (p_list)->head = (p_node);       \
                (p_list)->tail = (p_node);       \
            }                                    \
            else                                 \
            {                                    \
                (p_node)->prev = NULL;           \
                (p_node)->next = (p_list)->head; \
                                                 \
                /* Update head */                \
                (p_list)->head->prev = (p_node); \
                (p_list)->head       = (p_node); \
            }                                    \
        } while (0)

#define LIST_INSERT_TAIL(p_list, p_node)         \
        do {                                     \
            if ((p_list)->head == NULL)          \
            {                                    \
                (p_node)->prev = NULL;           \
                (p_node)->next = NULL;           \
                                                 \
                (p_list)->head = (p_node);       \
                (p_list)->tail = (p_node);       \
            }                                    \
            else                                 \
            {                                    \
                (p_node)->prev = (p_list)->tail; \
                (p_node)->next = NULL;           \
                                                 \
                /* Update tail */                \
                (p_list)->tail->next = (p_node); \
                (p_list)->tail       = (p_node); \
            }                                    \
        } while (0)

#define LIST_REMOVE_HEAD(p_list, p_head)           \
        do {                                       \
            /* If head == tail, node count is 1 */ \
            if ((p_list)->head == (p_list)->tail)  \
            {                                      \
                (p_list)->head = NULL;             \
                (p_list)->tail = NULL;             \
            }                                      \
            else                                   \
            {                                      \
                /* Move head node */               \
                (p_list)->head = (p_head)->next;   \
                                                   \
                /* Remove head arms */             \
                (p_head)->next->prev = NULL;       \
                (p_head)->next       = NULL;       \
            }                                      \
        } while (0)

#define LIST_REMOVE_TAIL(p_list, p_tail)           \
        do {                                       \
            /* If head == tail, node count is 1 */ \
            if ((p_list)->head == (p_list)->tail)  \
            {                                      \
                (p_list)->head = NULL;             \
                (p_list)->tail = NULL;             \
            }                                      \
            else                                   \
            {                                      \
                /* Move tail node */               \
                (p_list)->tail = (p_tail)->prev;   \
                                                   \
                /* Remove tail arms */             \
                (p_tail)->prev->next = NULL;       \
                (p_tail)->prev       = NULL;       \
            }                                      \
        } while (0)

#define LIST_REMOVE_NODE(p_list, p_node)                             \
        do {                                                         \
            /* If head == tail, node count is 1 */                   \
            if (p_list->head == p_list->tail)                        \
            {                                                        \
                /* 2 cases: p_node is head or not existed */         \
                if (p_list->head == p_node)                          \
                {                                                    \
                    p_list->head = NULL;                             \
                    p_list->tail = NULL;                             \
                }                                                    \
                else                                                 \
                {                                                    \
                    /* Return NULL means something wrong */          \
                    p_out_node = NULL;                               \
                }                                                    \
            }                                                        \
            else                                                     \
            {                                                        \
                /* 3 cases: p_node is head, tail, or middle */       \
                if (p_list->head == p_node)                          \
                {                                                    \
                    p_list->head = p_node->next;                     \
                                                                     \
                    /* Remove left arm */                            \
                    p_node->prev = NULL;                             \
                }                                                    \
                else if (p_list->tail == p_node)                     \
                {                                                    \
                    p_list->tail = p_node->prev;                     \
                                                                     \
                    /* Remove right arm */                           \
                    p_node->next = NULL;                             \
                }                                                    \
                else                                                 \
                {                                                    \
                    /* NOTE: user should check p_node is existing */ \
                    if (p_node->prev)                                \
                    {                                                \
                        p_node->prev->next = p_node->next;           \
                                                                     \
                        /* Remove left arm */                        \
                        p_node->prev = NULL;                         \
                    }                                                \
                    if (p_node->next)                                \
                    {                                                \
                        p_node->next->prev = p_node->prev;           \
                                                                     \
                        /* Remove right arm */                       \
                        p_node->next = NULL;                         \
                    }                                                \
                }                                                    \
            }                                                        \
        } while (0)

#define LIST_NODE_EXISTS(p_list, p_node, p_found) \
        do {                                      \
            __typeof__(p_node) p_iter = p_list->head; \
            while ((p_iter))                      \
            {                                     \
                if ((p_iter) == (p_node))         \
                {                                 \
                    *p_found = true;              \
                    break;                        \
                }                                 \
                (p_iter) = (p_iter)->next;        \
            }                                     \
        } while (0)

#endif
