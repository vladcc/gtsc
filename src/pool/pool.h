#ifndef POOL_H
#define POOL_H

#include "nslist/nslist.h"
#include <stddef.h>

// Module:      pool
// Description: Two-way memory pool. When initialized, it allocates enough
// memory for n objects of the same size. Then the user can request to get an
// object from the pool or put an object which came from the pool back in. If
// the pool's memory is exhausted, it allocates another memory region for the
// same number of objects. This two-way property makes it possible to avoid the
// overhead of malloc() and free() where that matters but also means that the
// pool needs to be able to somewhat manage the life cycle of its objects, hence
// the need of a constructor and destructor callbacks.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-07

typedef void (*pool_elem_ctor)(void * elem, void * arg);
typedef void (*pool_elem_dtor)(void * elem);

// Do not use directly.
typedef struct pool_void {
    nslist_void free_list;
    size_t elem_size;
    size_t elem_num;
    nslist_void pool_list;
    pool_elem_ctor ctor;
    void * ctor_arg;
    pool_elem_dtor dtor;
} pool_void;

// The pool allocates 'elem_size' * 'elem_num' bytes and calls 'ctor' for each
// element if 'ctor' is not NULL.
// Returns: 'pool' on success, NULL otherwise.
pool_void * pool_void_create(
    pool_void * pool,
    size_t elem_size,
    size_t elem_num,
    pool_elem_ctor ctor,
    void * ctor_arg,
    pool_elem_dtor dtor
);

// Like pool_void_create() but defaults the callbacks to NULL.
static inline pool_void * pool_void_create_pod(
    pool_void * pool,
    size_t elem_size,
    size_t elem_num
)
{
    return pool_void_create(pool, elem_size, elem_num, NULL, NULL, NULL);
}

// Releases all resources held by 'pool'. The destructor, if not NULL, is called
// for each element.
void pool_void_destroy(pool_void * pool);

// Returns: A pointer to a fixed size memory block.
void * pool_void_get(pool_void * pool);

// Puts 'elem' back in pool. 'elem' is assumed to point to an object which was
// obtained by pool_void_get().
void pool_void_put_back(pool_void * pool, void * elem);

// Generates type safe wrappers.
#define POOL_DEFINE(NAME, TYPE)                                                \
                                                                               \
typedef struct pool_##NAME {                                                   \
    pool_void pool;                                                            \
} pool_##NAME;                                                                 \
                                                                               \
static inline pool_##NAME * pool_##NAME##_create(                              \
    pool_##NAME * pool,                                                        \
    size_t elem_num,                                                           \
    pool_elem_ctor ctor,                                                       \
    void * ctor_arg,                                                           \
    pool_elem_dtor dtor                                                        \
)                                                                              \
{                                                                              \
    return (pool_##NAME *)pool_void_create((pool_void *)pool, sizeof(TYPE),    \
        elem_num, ctor, ctor_arg, dtor);                                       \
}                                                                              \
                                                                               \
static inline pool_##NAME * pool_##NAME##_create_pod(                          \
    pool_##NAME * pool,                                                        \
    size_t elem_num                                                            \
)                                                                              \
{                                                                              \
    return (pool_##NAME *)pool_void_create_pod((pool_void *)pool, sizeof(TYPE),\
        elem_num);                                                             \
}                                                                              \
                                                                               \
static inline void pool_##NAME##_destroy(pool_##NAME * pool)                   \
{                                                                              \
    pool_void_destroy((pool_void *)pool);                                      \
}                                                                              \
                                                                               \
static inline TYPE * pool_##NAME##_get(pool_##NAME * pool)                     \
{                                                                              \
    return (TYPE *)pool_void_get((pool_void *)pool);                           \
}                                                                              \
                                                                               \
static inline void pool_##NAME##_put_back(pool_##NAME * pool, TYPE * elem)     \
{                                                                              \
    pool_void_put_back((pool_void *)pool, (void *)elem);                       \
} typedef TYPE pool_##NAME##_dont_forget_the_semi

#endif
