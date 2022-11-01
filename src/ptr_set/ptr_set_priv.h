#ifndef PTR_SET_PRIV_H
#define PTR_SET_PRIV_H

#include "ptr_set_node.h"
#include <stddef.h>

// Module:      ptr_set
// Description: The private pool for ptr_set used to avoid malloc() and free().
// when chaining.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-29

typedef struct ptrs_priv_node_pool_node {
    ptrs_node * nodes;
    struct ptrs_priv_node_pool_node * next;
} ptrs_priv_node_pool_node;

typedef struct ptrs_priv_node_pool_list {
    ptrs_priv_node_pool_node * head;
} ptrs_priv_node_pool_list;

typedef struct ptrs_priv_node_list {
    ptrs_node * head;
} ptrs_priv_node_list;

typedef struct ptrs_priv_pool {
    ptrs_priv_node_list free_list;
    ptrs_priv_node_pool_list pool_list;
    size_t elem_num;
} ptrs_priv_pool;

ptrs_priv_pool * ptrs_priv_pool_create(ptrs_priv_pool * pool, size_t elem_num);
void ptrs_priv_pool_destroy(ptrs_priv_pool * pool);
ptrs_node * ptrs_priv_pool_get(ptrs_priv_pool * pool);
void ptrs_priv_pool_put_back(ptrs_priv_pool * pool, ptrs_node * elem);
const void * ptrs_priv_not_init_val(void);
ptrs_node * ptrs_priv_get_elem_addr(ptrs_priv_pool * pool, size_t elem);

static inline void * ptrs_priv_pool_get_head(ptrs_priv_pool * pool)
{
    return pool->free_list.head;
}

#define PTR_SET_PRIV_UNUSED(x) (void)(x)
static inline void * ptrs_priv_pool_next_get(
    ptrs_priv_pool * pool,
    const void * elem
)
{
    PTR_SET_PRIV_UNUSED(pool);
    return ((ptrs_node *)elem)->next;
}
#undef PTR_SET_PRIV_UNUSED

static inline size_t ptrs_priv_pool_curr_cap(ptrs_priv_pool * pool)
{
    return pool->elem_num;
}
#endif
