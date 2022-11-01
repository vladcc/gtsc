#ifndef OA_HASH_TBL_H
#define OA_HASH_TBL_H

#include "bit_set/bit_set.h"
#include <stddef.h>
#include <stdbool.h>

// Module:      oa_hash
// Description: Open address hash table implementation. Contains bit-wise copies
// of the inserted elements.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-18

// Load factor is expressed as a percent of the total table size. Once it's
// reached rehashing is initiated.
#define OAH_LOAD_FACT 70

// Three way comparison type.
typedef int (*oah_compar)(const void * tbl_key, const void * key);

// Hash function type.
typedef size_t (*oah_get_hash)(const void * key);

// Do not use directly.
typedef struct oah_void {
    void * data;
    bit_set is_taken;
    size_t key_size;
    size_t val_size;
    size_t val_offs;
    size_t kv_pair_size;
    size_t elem_left;
    size_t tbl_size;
    oah_get_hash hash;
    oah_compar cmp;
} oah_void;

// Creates a table which can hold 'elem_num' elements before it needs to grow.
// Returns: 'tbl' on success, NULL otherwise.
oah_void * oah_void_create_sz(
    oah_void * tbl,
    size_t key_size,
    size_t val_size,
    oah_get_hash hash,
    oah_compar cmp,
    size_t elem_num
);

// Like oah_void_create_sz(), but sets a default size.
static inline oah_void * oah_void_create(
    oah_void * tbl,
    size_t key_size,
    size_t val_size,
    oah_get_hash hash,
    oah_compar cmp
)
{
    return oah_void_create_sz(tbl, key_size, val_size, hash, cmp, 32);
}

// Releases the resources held by 'tbl'.
void oah_void_destroy(oah_void * tbl);

// If 'key' is not in 'tbl', copies the data at 'key' and 'val' and associates
// 'key' with 'val'. If 'key' is in 'tbl', rewrites 'val'.
// Returns: 'tbl' on success, NULL otherwise.
oah_void * oah_void_insert(oah_void * tbl, const void * key, const void * val);

// Returns: A pointer to the value associated with 'key' if 'key' is in 'tbl'.
// NULL otherwise.
void * oah_void_lookup(oah_void * tbl, const void * key);

// Returns: True if 'key' is in 'tbl', false otherwise.
static inline bool oah_void_has(oah_void * tbl, const void * key)
{
    return (oah_void_lookup(tbl, key) != NULL);
}

// Returns: the number of entries in 'tbl'.
size_t oah_void_entries(oah_void * tbl);

// Returns: True if 'tbl' is empty, false otherwise.
static inline bool oah_void_is_empty(oah_void * tbl)
{
    return (oah_void_entries(tbl) == 0);
}

// Removes 'key' and its associated value from 'tbl'.
// Returns: 'tbl'.
oah_void * oah_void_remove(oah_void * tbl, const void * key);

// Empties 'tbl' by setting its number of elements to 0.
// Returns: 'tbl'.
oah_void * oah_void_clear(oah_void * tbl);

// Returns: A oah_void struct. If 'is_ok' is true upon return, the returned
// struct holds a valid copy of 'tbl'.
oah_void oah_void_copy(oah_void * tbl, bool * is_ok);

// Keeps state during iteration,
typedef struct oah_iterator {
    size_t it;
} oah_iterator;
#define OAH_ITER_INIT {0}

// Iterates over 'tbl'. Each successive call moves to the next element. 'it' is
// assumed to have been initialized with OAH_ITER_INIT.
// Returns: A pointer to the next key in 'tbl', or NULL if there is no next key.
// 'out_val' points to the key's value.
const void * oah_void_iterate(
    oah_void * tbl,
    oah_iterator * it,
    void ** out_val
);

// Generates type safe wrappers.
#define OA_HASH_DEFINE(NAME, KEY_TYPE, VAL_TYPE)                               \
                                                                               \
typedef struct oah_##NAME {                                                    \
    oah_void tbl;                                                              \
} oah_##NAME;                                                                  \
                                                                               \
static inline oah_##NAME * oah_##NAME##_create_sz(                             \
    oah_##NAME * tbl,                                                          \
    oah_get_hash hash,                                                         \
    oah_compar cmp,                                                            \
    size_t elem_num                                                            \
)                                                                              \
{                                                                              \
    return (oah_##NAME *)oah_void_create_sz((oah_void *)tbl,                   \
        sizeof(KEY_TYPE), sizeof(VAL_TYPE), hash, cmp, elem_num);              \
}                                                                              \
                                                                               \
static inline oah_##NAME * oah_##NAME##_create(                                \
    oah_##NAME * tbl,                                                          \
    oah_get_hash hash,                                                         \
    oah_compar cmp                                                             \
)                                                                              \
{                                                                              \
    return (oah_##NAME *)oah_void_create((oah_void *)tbl,                      \
        sizeof(KEY_TYPE), sizeof(VAL_TYPE), hash, cmp);                        \
}                                                                              \
                                                                               \
static inline void oah_##NAME##_destroy(oah_##NAME * tbl)                      \
{                                                                              \
    oah_void_destroy((oah_void *)tbl);                                         \
}                                                                              \
                                                                               \
static inline const oah_##NAME * oah_##NAME##_insert(                          \
    oah_##NAME * tbl,                                                          \
    const KEY_TYPE * key,                                                      \
    const VAL_TYPE * val                                                       \
)                                                                              \
{                                                                              \
    return (oah_##NAME *)oah_void_insert((oah_void *)tbl, (const void *)key,   \
        (const void *)val);                                                    \
}                                                                              \
                                                                               \
static inline VAL_TYPE * oah_##NAME##_lookup(                                  \
    oah_##NAME * tbl,                                                          \
    const KEY_TYPE * key                                                       \
)                                                                              \
{                                                                              \
    return (VAL_TYPE *)oah_void_lookup((oah_void *)tbl, (const void *)key);    \
}                                                                              \
                                                                               \
static inline bool oah_##NAME##_has(oah_##NAME * tbl, const KEY_TYPE * key)    \
{                                                                              \
    return oah_void_has((oah_void *)tbl, (const void *)key);                   \
}                                                                              \
                                                                               \
static inline oah_##NAME * oah_##NAME##_remove(                                \
    oah_##NAME * tbl,                                                          \
    const KEY_TYPE * key                                                       \
)                                                                              \
{                                                                              \
    return (oah_##NAME *)oah_void_remove((oah_void *)tbl, (const void *)key);  \
}                                                                              \
                                                                               \
static inline oah_##NAME * oah_##NAME##_clear(oah_##NAME * tbl)                \
{                                                                              \
    return (oah_##NAME *)oah_void_clear((oah_void *)tbl);                      \
}                                                                              \
                                                                               \
static inline oah_##NAME oah_##NAME##_copy(oah_##NAME * tbl, bool * is_ok)     \
{                                                                              \
    oah_##NAME ret;                                                            \
    ret.tbl = oah_void_copy((oah_void *)tbl, is_ok);                           \
    return ret;                                                                \
}                                                                              \
                                                                               \
static inline const KEY_TYPE * oah_##NAME##_iterate(                           \
    oah_##NAME * tbl,                                                          \
    oah_iterator * it,                                                         \
    VAL_TYPE ** out_val                                                        \
)                                                                              \
{                                                                              \
    return (const KEY_TYPE *)oah_void_iterate((oah_void *)tbl, it,             \
        (void **)out_val);                                                     \
}                                                                              \
                                                                               \
static inline size_t oah_##NAME##_entries(oah_##NAME * tbl)                    \
{                                                                              \
     return oah_void_entries((oah_void *)tbl);                                 \
}                                                                              \
                                                                               \
static inline bool oah_##NAME##_is_empty(oah_##NAME * tbl)                     \
{                                                                              \
    return oah_void_is_empty((oah_void *)tbl);                                 \
} typedef KEY_TYPE oa_hash_##NAME##_dont_forget_the_semi

#endif
