#ifndef GROW_POOL_H
#define GROW_POOL_H

#include "vect/vect.h"

// Module:      grow_pool
// Description: A two-way pool implemented with a vector. The user can take
// elements out and put then back in. Once the capacity of the pool is reached,
// it grows by a factor of two. Because the elements may move, they can only
// be referenced by their gpool_id.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-17

typedef void (*fvoid)(void);
typedef void (*gpool_elem_ctor)(void * elem, void * arg);
typedef void (*gpool_elem_dtor)(void * elem);

// Do not use directly.
typedef struct grow_pool_void {
    vect_void vect;
    fvoid ctor;
    void * ctor_arg;
    fvoid dtor;
} grow_pool_void;

// Identifies an element from the pool. Wrapped so pool functions do not take
// any number given to them.
typedef struct gpool_id {
    size_t id;
} gpool_id;

// Creates a grow pool of 'elem_num' elements each of 'elem_size'. 'ctor' and
// 'dtor' are called, if not NULL, for every element on allocation and
// destruction of the pool respectively.
// Returns: 'pool' on success, NULL otherwise.
grow_pool_void * gpool_void_create_sz(
    grow_pool_void * pool,
    size_t elem_size,
    size_t elem_num,
    gpool_elem_ctor ctor,
    void * ctor_arg,
    gpool_elem_dtor dtor
);

// Like gpool_void_create_sz() but without callbacks.
static inline grow_pool_void * gpool_void_create_pod(
    grow_pool_void * pool,
    size_t elem_size,
    size_t elem_num
)
{
    return gpool_void_create_sz(pool, elem_size, elem_num, NULL, NULL, NULL);
}

// Releases the resources held by 'pool'.
void gpool_void_destroy(grow_pool_void * pool);

// Takes an element out of the pool and gives it to the user.
// Returns: A handle to the next element in the pool.
gpool_id gpool_void_get(grow_pool_void * pool);

// Returns: The address of the element specified by 'id' in 'pool'.
static inline void * gpool_void_reach(grow_pool_void * pool, gpool_id id)
{
    return vect_void_get(&pool->vect, id.id);
}

// Returns the element specified by 'id' to 'pool'. 'id' is assumed to have been
// obtained by a call to gpool_void_get().
void gpool_void_put_back(grow_pool_void * pool, gpool_id id);

// Returns: The element size of 'pool'.
static inline size_t gpool_void_elem_size(grow_pool_void * pool)
{
    return vect_void_elem_size(&pool->vect);
}

// Returns: How many elements 'pool' holds both used and unused.
static inline size_t gpool_void_elem_num(grow_pool_void * pool)
{
    return vect_void_cap(&pool->vect);
}

// Generates type safe wrappers.
#define GROW_POOL_DEFINE(NAME, TYPE)                                           \
                                                                               \
typedef struct grow_pool_##NAME {                                              \
    grow_pool_void pool;                                                       \
} grow_pool_##NAME;                                                            \
                                                                               \
static inline grow_pool_##NAME * gpool_##NAME##_create_sz(                     \
    grow_pool_##NAME * pool,                                                   \
    size_t elem_num,                                                           \
    gpool_elem_ctor ctor,                                                      \
    void * ctor_arg,                                                           \
    gpool_elem_dtor dtor                                                       \
)                                                                              \
{                                                                              \
    return (grow_pool_##NAME *)gpool_void_create_sz((grow_pool_void *)pool,    \
        sizeof(TYPE), elem_num, ctor, ctor_arg, dtor);                         \
}                                                                              \
                                                                               \
static inline grow_pool_##NAME * gpool_##NAME##_create_pod(                    \
    grow_pool_##NAME * pool,                                                   \
    size_t elem_num                                                            \
)                                                                              \
{                                                                              \
    return (grow_pool_##NAME *)gpool_void_create_pod((grow_pool_void *)pool,   \
        sizeof(TYPE), elem_num);                                               \
}                                                                              \
                                                                               \
static inline void gpool_##NAME##_destroy(grow_pool_##NAME * pool)             \
{                                                                              \
    gpool_void_destroy((grow_pool_void *)pool);                                \
}                                                                              \
                                                                               \
static inline gpool_id gpool_##NAME##_get(grow_pool_##NAME * pool)             \
{                                                                              \
    return gpool_void_get((grow_pool_void *)pool);                             \
}                                                                              \
                                                                               \
static inline TYPE * gpool_##NAME##_reach(grow_pool_##NAME * pool, gpool_id id)\
{                                                                              \
    return (TYPE *)gpool_void_reach((grow_pool_void *)pool, id);               \
}                                                                              \
                                                                               \
static inline void gpool_##NAME##_put_back(                                    \
    grow_pool_##NAME * pool,                                                   \
    gpool_id id                                                                \
)                                                                              \
{                                                                              \
    gpool_void_put_back((grow_pool_void *)pool, id);                           \
}                                                                              \
                                                                               \
static inline size_t gpool_##NAME##_elem_size(grow_pool_##NAME * pool)         \
{                                                                              \
    return gpool_void_elem_size((grow_pool_void *)pool);                       \
}                                                                              \
                                                                               \
static inline size_t gpool_##NAME##_elem_num(grow_pool_##NAME * pool)          \
{                                                                              \
    return gpool_void_elem_num((grow_pool_void *)pool);                        \
} typedef TYPE gpool_##NAME##_dont_forget_the_semi

#endif
