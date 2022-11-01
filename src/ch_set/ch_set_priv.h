#ifndef CH_SET_PRIV_H
#define CH_SET_PRIV_H

#include "nslist/nslist.h"
#include <stddef.h>

typedef struct chs_priv_pool {
    nslist_void free_list;
    size_t elem_size;
    size_t elem_num;
    nslist_void pool_list;
} chs_priv_pool;

chs_priv_pool * chs_priv_pool_create(
    chs_priv_pool * pool,
    size_t elem_size,
    size_t elem_num
);
void chs_priv_pool_destroy(chs_priv_pool * pool);
void * chs_priv_pool_get(chs_priv_pool * pool);
void chs_priv_pool_put_back(chs_priv_pool * pool, void * elem);
const void * chs_priv_not_init_val(void);
void * chs_priv_get_elem_addr(chs_priv_pool * pool, size_t elem);

static inline size_t chs_priv_pool_elem_size(chs_priv_pool * pool)
{
    return pool->elem_size;
}
static inline void * chs_priv_pool_get_head(chs_priv_pool * pool)
{
    return nslist_void_head(&pool->free_list);
}

static inline void * chs_priv_pool_next_get(chs_priv_pool * pool, void * elem)
{
    return nslist_void_next(&pool->free_list, elem);
}
static inline void chs_priv_pool_next_set(
    chs_priv_pool * pool,
    void * elem,
    void * next
)
{
    *nslist_void_pnext(&pool->free_list, elem) = next;
}
static inline size_t chs_priv_pool_curr_cap(chs_priv_pool * pool)
{
    return pool->elem_num;
}
#endif
