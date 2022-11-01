#ifndef ONE_WAY_POOL_H
#define ONE_WAY_POOL_H

#include "nslist/nslist.h"
#include <stddef.h>

// Module:      one_way_pool
// Description: Simplest pool of all. Allocates memory for n elements, gives
// them out to the user. The memory can only be released by destroying the pool.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-17

// Do not use directly.
typedef struct owp_void {
    nslist_void pool_list;
    size_t pos;
    size_t elem_size;
    size_t elem_num;
} owp_void;

// Creates a pool of 'elem_num' 'elem_size' elements.
// Returns: 'pool' on success, NULL otherwise.
owp_void * owp_void_create(owp_void * pool, size_t elem_size, size_t elem_num);

// Releases the resources held by 'pool'.
void owp_void_destroy(owp_void * pool);

// Returns: The address of the next element.
void * owp_void_get(owp_void * pool);

// Generates type safe wrappers.
#define OWP_DEFINE(NAME, TYPE)                                                 \
                                                                               \
typedef struct owp_##NAME {                                                    \
    owp_void pool;                                                             \
} owp_##NAME;                                                                  \
                                                                               \
static inline owp_##NAME * owp_##NAME##_create(                                \
    owp_##NAME * pool,                                                         \
    size_t elem_num                                                            \
)                                                                              \
{                                                                              \
    return (owp_##NAME *)owp_void_create((owp_void *)pool, sizeof(TYPE),       \
        elem_num);                                                             \
}                                                                              \
                                                                               \
static inline void owp_##NAME##_destroy(owp_##NAME * pool)                     \
{                                                                              \
    owp_void_destroy(&pool->pool);                                             \
}                                                                              \
                                                                               \
static inline TYPE * owp_##NAME##_get(owp_##NAME * pool)                       \
{                                                                              \
    return (TYPE *)owp_void_get(&pool->pool);                                  \
} typedef TYPE owp_##NAME##_dont_forget_the_semi

#endif
