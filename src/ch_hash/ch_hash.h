#ifndef CH_HASH_H
#define CH_HASH_H

#include "ch_hash_priv.h"
#include <stdbool.h>

// Module:      ch_hash
// Description: Chained hash table implementation. Contains bit-wise copies of
// the inserted data. Uses chaining to resolve collisions. However, it does not
// guarantee the elements won't move, therefore don't hold on to the returned
// addresses. The load factor is always one. When it's reached, rehashing is
// initiated.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-18

// Three way comparison type.
typedef int (*chh_compar)(const void * tbl_elem, const void * key);

// Hash function type.
typedef size_t (*chh_get_hash)(const void * elem);

// Do not use directly.
typedef struct chh_void {
    unsigned char * data;
    size_t key_size;
    size_t val_size;
    size_t val_offs;
    size_t elems;
    size_t tbl_size;
    chh_compar cmp;
    chh_get_hash hash;
    chh_priv_pool chain_pool;
} chh_void;

#define CH_HASH_DEFAULT_POOL 16
#define CH_HASH_DEFAULT_SIZE 32

// Creates a table large enough for 'elem_num' of elements.
// Returns: 'tbl' on success, NULL otherwise.
chh_void * chh_void_create_sz_pl(
    chh_void * tbl,
    size_t key_size,
    size_t val_size,
    chh_get_hash hash,
    chh_compar cmp,
    size_t elem_num,
    size_t pool_sz
);

// Like chh_void_create_sz_pl() but with default pool size.
static inline chh_void * chh_void_create_sz(
    chh_void * tbl,
    size_t key_size,
    size_t val_size,
    chh_get_hash hash,
    chh_compar cmp,
    size_t elem_num
)
{
    return chh_void_create_sz_pl(tbl, key_size, val_size, hash, cmp, elem_num,
        CH_HASH_DEFAULT_POOL);
}

// Like chh_void_create_sz() but provides a default size.
static inline chh_void * chh_void_create(
    chh_void * tbl,
    size_t key_size,
    size_t val_size,
    chh_get_hash hash,
    chh_compar cmp
)
{
    return chh_void_create_sz(tbl, key_size, val_size, hash, cmp,
        CH_HASH_DEFAULT_SIZE);
}

// Releases the resources held by 'tbl'.
void chh_void_destroy(chh_void * tbl);

// Inserts 'key' and 'val' in 'tbl'. If 'key' is already in 'tbl', 'val' is
// overwritten.
// Returns: 'tbl' on success, NULL otherwise.
chh_void * chh_void_insert(chh_void * tbl, const void * key, const void * val);

// Returns: A pointer to the value of 'key', NULL if 'key' is not in 'tbl'.
void * chh_void_lookup(chh_void * tbl, const void * key);

// Removes 'key' and its value from 'tbl'.
// Returns: 'tbl'.
chh_void * chh_void_remove(chh_void * tbl, const void * key);

// Empties 'tbl'.
// Returns: 'tbl'.
chh_void * chh_void_clear(chh_void * tbl);

// Returns: A chh_void struct. If 'is_ok' is true upon return, the returned
// struct holds a valid copy of 'tbl'.
chh_void chh_void_copy(chh_void * tbl, bool * is_ok);

// Returns: The number of elements in 'tbl'.
static inline size_t chh_void_elems(chh_void * tbl)
{
    return tbl->elems;
}

// Returns: True if 'tbl' is empty, false otherwise.
static inline bool chh_void_is_empty(chh_void * tbl)
{
    return (chh_void_elems(tbl) == 0);
}

// Returns: The max number of elements 'tbl' can hold before it has to grow.
static inline size_t chh_void_cap(chh_void * tbl)
{
    return tbl->tbl_size;
}

// Keeps state during iteration.
typedef struct chh_iterator {
    size_t bucket;
    unsigned char * node;
} chh_iterator;

// Initializes 'iter'.
static inline void chh_iterator_init(chh_iterator * iter)
{
    iter->bucket = 0;
    iter->node = NULL;
}

// Iterates over 'tbl'. 'it' is assumed to have been initialized by
// chh_iterator_init().
// Returns: A pointer to the next key in 'tbl', NULL if there is no next key.
// 'out_val' points to the key's value.
const void * chh_void_iterate(
    chh_void * tbl,
    chh_iterator * it,
    void ** out_val
);

// Generates type safe wrappers.
#define CH_HASH_DEFINE(NAME, KEY_TYPE, VAL_TYPE)                               \
                                                                               \
typedef struct chh_##NAME {                                                    \
    chh_void tbl;                                                              \
} chh_##NAME;                                                                  \
                                                                               \
static inline chh_##NAME * chh_##NAME##_create_sz_pl(                          \
    chh_##NAME * tbl,                                                          \
    chh_get_hash hash,                                                         \
    chh_compar cmp,                                                            \
    size_t elem_num,                                                           \
    size_t pool_sz                                                             \
)                                                                              \
{                                                                              \
    return (chh_##NAME *)chh_void_create_sz_pl((chh_void *)tbl,                \
        sizeof(KEY_TYPE), sizeof(VAL_TYPE), hash, cmp, elem_num, pool_sz);     \
}                                                                              \
                                                                               \
static inline chh_##NAME * chh_##NAME##_create_sz(                             \
    chh_##NAME * tbl,                                                          \
    chh_get_hash hash,                                                         \
    chh_compar cmp,                                                            \
    size_t elem_num                                                            \
)                                                                              \
{                                                                              \
    return (chh_##NAME *)chh_void_create_sz((chh_void *)tbl,                   \
        sizeof(KEY_TYPE), sizeof(VAL_TYPE), hash, cmp, elem_num);              \
}                                                                              \
                                                                               \
static inline chh_##NAME * chh_##NAME##_create(                                \
    chh_##NAME * tbl,                                                          \
    chh_get_hash hash,                                                         \
    chh_compar cmp                                                             \
)                                                                              \
{                                                                              \
    return (chh_##NAME *)chh_void_create((chh_void *)tbl,                      \
        sizeof(KEY_TYPE), sizeof(VAL_TYPE), hash, cmp);                        \
}                                                                              \
                                                                               \
static inline void chh_##NAME##_destroy(chh_##NAME * tbl)                      \
{                                                                              \
    chh_void_destroy((chh_void *)tbl);                                         \
}                                                                              \
                                                                               \
static inline chh_##NAME * chh_##NAME##_insert(                                \
    chh_##NAME * tbl,                                                          \
    const KEY_TYPE * key,                                                      \
    const VAL_TYPE * val                                                       \
)                                                                              \
{                                                                              \
    return (chh_##NAME *)chh_void_insert((chh_void *)tbl, (const void *)key,   \
        (const void *)val);                                                    \
}                                                                              \
                                                                               \
static inline VAL_TYPE * chh_##NAME##_lookup(                                  \
    chh_##NAME * tbl,                                                          \
    const KEY_TYPE * key                                                       \
)                                                                              \
{                                                                              \
    return (VAL_TYPE *)chh_void_lookup((chh_void *)tbl, (const void *)key);    \
}                                                                              \
                                                                               \
static inline chh_##NAME * chh_##NAME##_remove(                                \
    chh_##NAME * tbl,                                                          \
    const KEY_TYPE * key                                                       \
)                                                                              \
{                                                                              \
    return (chh_##NAME *)chh_void_remove((chh_void *)tbl, (const void *)key);  \
}                                                                              \
                                                                               \
static inline chh_##NAME * chh_##NAME##_clear(chh_##NAME * tbl)                \
{                                                                              \
    return (chh_##NAME *)chh_void_clear((chh_void *)tbl);                      \
}                                                                              \
                                                                               \
static inline chh_##NAME chh_##NAME##_copy(chh_##NAME * tbl, bool * is_ok)     \
{                                                                              \
    chh_##NAME ret;                                                            \
    ret.tbl = chh_void_copy((chh_void *)tbl, is_ok);                           \
    return ret;                                                                \
}                                                                              \
                                                                               \
static inline size_t chh_##NAME##_elems(chh_##NAME * tbl)                      \
{                                                                              \
    return chh_void_elems((chh_void *)tbl);                                    \
}                                                                              \
                                                                               \
static inline bool chh_##NAME##_is_empty(chh_##NAME * tbl)                     \
{                                                                              \
    return chh_void_is_empty((chh_void *)tbl);                                 \
}                                                                              \
                                                                               \
static inline const KEY_TYPE * chh_##NAME##_iterate(                           \
    chh_##NAME * tbl,                                                          \
    chh_iterator * it,                                                         \
    VAL_TYPE ** out_val                                                        \
)                                                                              \
{                                                                              \
    return (const KEY_TYPE *)chh_void_iterate((chh_void *)tbl,                 \
        it, (void **)out_val);                                                 \
}                                                                              \
                                                                               \
static inline size_t chh_##NAME##_cap(chh_##NAME * tbl)                        \
{                                                                              \
    return chh_void_cap((chh_void *)tbl);                                      \
} typedef KEY_TYPE chh_hash_##NAME##_dont_forget_the_semi

#endif
