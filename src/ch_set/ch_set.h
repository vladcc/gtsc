#ifndef CH_SET_H
#define CH_SET_H

#include "ch_set_priv.h"
#include <stdbool.h>

// Module:      ch_set
// Description: Chained set hash table implementation. Contains bit-wise copies
// of the inserted data. Uses chaining to resolve collisions. However, it does
// not guarantee the elements won't move, therefore don't hold on to the
// returned addresses. The load factor is always one. When it's reached,
// rehashing is initiated.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-29

// Three way comparison type.
typedef int (*chs_compar)(const void * tbl_elem, const void * key);

// Hash function type.
typedef size_t (*chs_get_hash)(const void * elem);

// Do not use directly.
typedef struct chs_void {
    unsigned char * data;
    size_t key_size;
    size_t elems;
    size_t tbl_size;
    chs_compar cmp;
    chs_get_hash hash;
    chs_priv_pool chain_pool;
} chs_void;

#define CH_SET_DEFAULT_POOL 16
#define CH_SET_DEFAULT_SIZE 32

// Creates a table large enough for 'elem_num' of elements. 'pool_sz' determines
// how many elements the internal chain resolution pool allocates at once.
// Returns: 'tbl' on success, NULL otherwise.
chs_void * chs_void_create_sz_pl(
    chs_void * tbl,
    size_t key_size,
    chs_get_hash hash,
    chs_compar cmp,
    size_t elem_num,
    size_t pool_sz
);

// Like chs_void_create_sz_pl() but with default pool size.
static inline chs_void * chs_void_create_sz(
    chs_void * tbl,
    size_t key_size,
    chs_get_hash hash,
    chs_compar cmp,
    size_t elem_num
)
{
    return chs_void_create_sz_pl(tbl, key_size, hash, cmp, elem_num,
        CH_SET_DEFAULT_POOL);
}

// Like chs_void_create_sz() but provides a default size.
static inline chs_void * chs_void_create(
    chs_void * tbl,
    size_t key_size,
    chs_get_hash hash,
    chs_compar cmp
)
{
    return chs_void_create_sz(tbl, key_size, hash, cmp, CH_SET_DEFAULT_SIZE);
}

// Releases the resources held by 'tbl'.
void chs_void_destroy(chs_void * tbl);

// Inserts 'key' in 'tbl'. If 'key' is already in 'tbl' nothing happens.
// Returns: 'tbl' on success, NULL otherwise.
chs_void * chs_void_insert(chs_void * tbl, const void * key);

// Returns: A pointer to the key inside 'tbl', NULL if 'key' is not in 'tbl'.
const void * chs_void_lookup(chs_void * tbl, const void * key);

// Removes 'key' from 'tbl'.
// Returns: 'tbl'.
chs_void * chs_void_remove(chs_void * tbl, const void * key);

// Empties 'tbl'.
// Returns: 'tbl'.
chs_void * chs_void_clear(chs_void * tbl);

// Returns: A chs_void struct. If 'is_ok' is true upon return, the returned
// struct holds a valid copy of 'tbl'.
chs_void chs_void_copy(chs_void * tbl, bool * is_ok);

// Returns: The number of elements in 'tbl'.
static inline size_t chs_void_elems(chs_void * tbl)
{
    return tbl->elems;
}

// Returns: True if 'tbl' is empty, false otherwise.
static inline bool chs_void_is_empty(chs_void * tbl)
{
    return (chs_void_elems(tbl) == 0);
}

// Returns: The max number of elements 'tbl' can hold before it has to grow.
static inline size_t chs_void_cap(chs_void * tbl)
{
    return tbl->tbl_size;
}

// Keeps state during iteration.
typedef struct chs_iterator {
    size_t bucket;
    unsigned char * node;
} chs_iterator;

// Initializes 'iter'.
static inline void chs_iterator_init(chs_iterator * iter)
{
    iter->bucket = 0;
    iter->node = NULL;
}

// Iterates over 'tbl'. 'it' is assumed to have been initialized by
// chs_iterator_init().
// Returns: A pointer to the next key in 'tbl', NULL if there is no next key.
const void * chs_void_iterate(
    chs_void * tbl,
    chs_iterator * it
);

// Generates type safe wrappers.
#define CH_SET_DEFINE(NAME, KEY_TYPE)                                          \
                                                                               \
typedef struct chs_##NAME {                                                    \
    chs_void tbl;                                                              \
} chs_##NAME;                                                                  \
                                                                               \
static inline chs_##NAME * chs_##NAME##_create_sz_pl(                          \
    chs_##NAME * tbl,                                                          \
    chs_get_hash hash,                                                         \
    chs_compar cmp,                                                            \
    size_t elem_num,                                                           \
    size_t pool_sz                                                             \
)                                                                              \
{                                                                              \
    return (chs_##NAME *)chs_void_create_sz_pl((chs_void *)tbl,                \
        sizeof(KEY_TYPE), hash, cmp, elem_num, pool_sz);                       \
}                                                                              \
                                                                               \
static inline chs_##NAME * chs_##NAME##_create_sz(                             \
    chs_##NAME * tbl,                                                          \
    chs_get_hash hash,                                                         \
    chs_compar cmp,                                                            \
    size_t elem_num                                                            \
)                                                                              \
{                                                                              \
    return (chs_##NAME *)chs_void_create_sz((chs_void *)tbl,                   \
        sizeof(KEY_TYPE), hash, cmp, elem_num);                                \
}                                                                              \
                                                                               \
static inline chs_##NAME * chs_##NAME##_create(                                \
    chs_##NAME * tbl,                                                          \
    chs_get_hash hash,                                                         \
    chs_compar cmp                                                             \
)                                                                              \
{                                                                              \
    return (chs_##NAME *)chs_void_create((chs_void *)tbl,                      \
        sizeof(KEY_TYPE), hash, cmp);                                          \
}                                                                              \
                                                                               \
static inline void chs_##NAME##_destroy(chs_##NAME * tbl)                      \
{                                                                              \
    chs_void_destroy((chs_void *)tbl);                                         \
}                                                                              \
                                                                               \
static inline chs_##NAME * chs_##NAME##_insert(                                \
    chs_##NAME * tbl,                                                          \
    const KEY_TYPE * key                                                       \
)                                                                              \
{                                                                              \
    return (chs_##NAME *)chs_void_insert((chs_void *)tbl, (const void *)key);  \
}                                                                              \
                                                                               \
static inline const KEY_TYPE * chs_##NAME##_lookup(                            \
    chs_##NAME * tbl,                                                          \
    const KEY_TYPE * key                                                       \
)                                                                              \
{                                                                              \
    return (const KEY_TYPE *)chs_void_lookup((chs_void *)tbl,                  \
        (const void *)key);                                                    \
}                                                                              \
                                                                               \
static inline chs_##NAME * chs_##NAME##_remove(                                \
    chs_##NAME * tbl,                                                          \
    const KEY_TYPE * key                                                       \
)                                                                              \
{                                                                              \
    return (chs_##NAME *)chs_void_remove((chs_void *)tbl, (const void *)key);  \
}                                                                              \
                                                                               \
static inline chs_##NAME * chs_##NAME##_clear(chs_##NAME * tbl)                \
{                                                                              \
    return (chs_##NAME *)chs_void_clear((chs_void *)tbl);                      \
}                                                                              \
                                                                               \
static inline chs_##NAME chs_##NAME##_copy(chs_##NAME * tbl, bool * is_ok)     \
{                                                                              \
    chs_##NAME ret;                                                            \
    ret.tbl = chs_void_copy((chs_void *)tbl, is_ok);                           \
    return ret;                                                                \
}                                                                              \
                                                                               \
static inline size_t chs_##NAME##_elems(chs_##NAME * tbl)                      \
{                                                                              \
    return chs_void_elems((chs_void *)tbl);                                    \
}                                                                              \
                                                                               \
static inline bool chs_##NAME##_is_empty(chs_##NAME * tbl)                     \
{                                                                              \
    return chs_void_is_empty((chs_void *)tbl);                                 \
}                                                                              \
                                                                               \
static inline const KEY_TYPE * chs_##NAME##_iterate(                           \
    chs_##NAME * tbl,                                                          \
    chs_iterator * it                                                          \
)                                                                              \
{                                                                              \
    return (const KEY_TYPE *)chs_void_iterate((chs_void *)tbl, it);            \
}                                                                              \
                                                                               \
static inline size_t chs_##NAME##_cap(chs_##NAME * tbl)                        \
{                                                                              \
    return chs_void_cap((chs_void *)tbl);                                      \
} typedef KEY_TYPE chs_hash_##NAME##_dont_forget_the_semi

#endif
