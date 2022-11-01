#ifndef VECT_H
#define VECT_H

#include "arr_algo/arr_algo_defn.h"
#include <string.h>
#include <stdbool.h>

// Module:      vect
// Description: A dynamic array implementation.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-02

// Do not use directly.
typedef struct vect_void {
    void * data;
    size_t len;
    size_t cap;
    size_t elem_size;
} vect_void;

// The arr_algo wrappers follow below. No details are given here. For that
// information please refer to the arr_algo module. Note that more functions
// follow after the wrappers.
void vect_void_range_set_to(
    vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key
);
static inline void vect_void_set_to(vect_void * v, const void * key)
{
    vect_void_range_set_to(v, 0, v->len, key);
}

// Creates a vector which contains a copy of the specified range in 'src'.
// Returns: A vect structure. 'is_ok' is true of the operation succeeded, false
// otherwise.
vect_void vect_void_range_copy(
    const vect_void * src,
    size_t start,
    size_t end_non_incl,
    bool * out_ok
);

// Like vect_void_range_copy() but copies the whole 'src'.
static inline vect_void vect_void_copy(const vect_void * src, bool * out_ok)
{
    return vect_void_range_copy(src, 0, src->len, out_ok);
}

bool vect_void_range_is_sorted(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    arr_fcomp cmp
);
static inline bool vect_void_is_sorted(const vect_void * v, arr_fcomp cmp)
{
    return vect_void_range_is_sorted(v, 0, v->len, cmp);
}

void vect_void_range_sort(
    vect_void * v,
    size_t start,
    size_t end_non_incl,
    arr_fcomp cmp
);
static inline void vect_void_sort(vect_void * v, arr_fcomp cmp)
{
    vect_void_range_sort(v, 0, v->len, cmp);
}

void * vect_void_range_find(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
);
static inline void * vect_void_find(
    const vect_void * v,
    const void * key,
    arr_fcomp cmp
)
{
    return vect_void_range_find(v, 0, v->len, key, cmp);
}

const void * vect_void_range_bsearch(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
);
static inline const void * vect_void_bsearch(
    const vect_void * v,
    const void * key,
    arr_fcomp cmp
)
{
    return vect_void_range_bsearch(v, 0, v->len, key, cmp);
}

const void * vect_void_range_sfirst_eq(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
);
static inline const void * vect_void_sfirst_eq(
    const vect_void * v,
    const void * key,
    arr_fcomp cmp
)
{
    return vect_void_range_sfirst_eq(v, 0, v->len, key, cmp);
}

const void * vect_void_range_sfirst_geq(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
);
static inline const void * vect_void_sfirst_geq(
    const vect_void * v,
    const void * key,
    arr_fcomp cmp
)
{
    return vect_void_range_sfirst_geq(v, 0, v->len, key, cmp);
}

const void * vect_void_range_sfirst_gt(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
);
static inline const void * vect_void_sfirst_gt(
    const vect_void * v,
    const void * key,
    arr_fcomp cmp
)
{
    return vect_void_range_sfirst_gt(v, 0, v->len, key, cmp);
}

bool vect_void_is_eq(const vect_void * a, const vect_void * b, arr_fcomp cmp);

void vect_void_swap(vect_void * v, size_t a, size_t b, void * elem_tmp);

void vect_void_range_reverse(
    vect_void * v,
    size_t start,
    size_t end_non_incl,
    void * elem_tmp
);
static inline void vect_void_reverse(vect_void * v, void * elem_tmp)
{
    vect_void_range_reverse(v, 0, v->len, elem_tmp);
}

void vect_void_range_apply(
    vect_void * v,
    size_t start,
    size_t end_non_incl,
    arr_fapply fn,
    void * arg
);
static inline void vect_void_apply(vect_void * v, arr_fapply fn, void * arg)
{
    vect_void_range_apply(v, 0, v->len, fn, arg);
}

void * vect_void_range_partition(
    vect_void * v,
    size_t start,
    size_t end_non_incl,
    arr_fpred pred,
    void * arg,
    void * elem_tmp
);
static inline void * vect_void_partition(
    vect_void * v,
    arr_fpred pred,
    void * arg,
    void * elem_tmp
)
{
    return vect_void_range_partition(v, 0, v->len, pred, arg, elem_tmp);
}

// vect specific functions follow below.

// Creates a vector with 'elem_size' element size and 'cap' capacity.
// Returns: 'v' on success, NULL otherwise.
vect_void * vect_void_create_cap(vect_void * v, size_t elem_size, size_t cap);

// Like vect_void_create_cap() but provides a default capacity.
static inline vect_void * vect_void_create(vect_void * v, size_t elem_size)
{
    return vect_void_create_cap(v, elem_size, 32);
}

// Appends 'num' number of elements from the array pointed to by 'src' to 'v'.
// The elements in 'src' are assumed to be the same size as the elements in 'v'.
// Returns: 'v' on success, NULL otherwise.
vect_void * vect_void_append(vect_void * v, const void * src, size_t num);

// Create + append.
// Returns: 'v' on success, NULL otherwise.
static inline vect_void * vect_void_create_arr(
    vect_void  * v,
    size_t elem_size,
    const void * src,
    size_t num
)
{
    return (vect_void_create_cap(v, elem_size, num) &&
        vect_void_append(v, src, num)) ? v : NULL;
}

// Frees the resources associated with 'v'.
void vect_void_destroy(vect_void * v);

// Sets the capacity of 'v' to 'cap'. If 'cap' is less than the current length,
// the length is trimmed to 'cap'.
// Returns: 'v' on success, NULL otherwise.
vect_void * vect_void_resize(vect_void * v, size_t cap);

// Like vect_void_resize() but sets the capacity of 'v' equal to its length.
static inline vect_void * vect_void_shrink_to_fit(vect_void * v)
{
    return vect_void_resize(v, v->len);
}

// Returns: The address of the first member of 'v'.
static inline void * vect_void_data(const vect_void * v)
{
    return v->data;
}

// Returns: The current length of 'v'.
static inline size_t vect_void_len(const vect_void * v)
{
    return v->len;
}

// Returns: The current capacity of 'v'.
static inline size_t vect_void_cap(const vect_void * v)
{
    return v->cap;
}

// Returns: The current element size of 'v'.
static inline size_t vect_void_elem_size(const vect_void * v)
{
    return v->elem_size;
}

// Returns: True if 'v' is empty, false otherwise.
static inline bool vect_void_is_empty(const vect_void * v)
{
    return !v->len;
}

// Returns: The index of the last element of 'v'.
static inline size_t vect_void_last(const vect_void * v)
{
    return v->len-1;
}

// Returns: The address of the element at 'index' in 'v'.
static inline void * vect_void_get(const vect_void * v, size_t index)
{
    return (unsigned char *)v->data + index * v->elem_size;
}

// Sets the length of 'v' to 'len'.
static inline void vect_void_set_len(vect_void * v, size_t len)
{
    v->len = len;
}

// Zeroes out the whole capacity of 'v'.
static inline void vect_void_cap_zero(vect_void * v)
{
    memset(v->data, 0, v->cap*v->elem_size);
}

// Zeroes out the length of 'v'.
static inline void vect_void_len_zero(vect_void * v)
{
    memset(v->data, 0, v->len*v->elem_size);
}

// Returns: the distance between 'result' and the address of the first member
// of 'v' in array units. 'result' is assumed to point somewhere inside 'v'.
static inline size_t vect_void_distance(
    const vect_void * v,
    const void * result
)
{
    return ((unsigned char *)result - (unsigned char *)v->data) / v->elem_size;
}

// Copies the data from 'elem' to index 'index' in 'v'. 'elem' is assumed to
// point to an element with the same type as the elements in 'v'.
// Returns: The address of the element at 'index'.
static inline void * vect_void_write(
    vect_void * v,
    size_t index,
    const void * elem
)
{
    return memcpy(vect_void_get(v, index), elem, v->elem_size);
}

// Pushes the data at 'elem' to the back of 'v'.
// Returns: The address of the element just pushed on success, NULL otherwise.
void * vect_void_push(vect_void * v, const void * elem);

// Removes the last element of 'v'.
static inline void vect_void_pop(vect_void * v)
{
    --v->len;
}

// Removes 'n' number of elements from the back of 'v'.
static inline void vect_void_pop_n(vect_void * v, size_t n)
{
    v->len -= n;
}

// Empties 'v'.
static inline void vect_void_reset(vect_void * v)
{
    v->len = 0;
}

// Returns: the address of the last element in 'v'.
static inline void * vect_void_peek(const vect_void * v)
{
    return vect_void_get(v, v->len-1);
}

// Swaps the element at 'index' with the last element and pops, therefore
// removing the element at 'index'. 'elem_tmp' is a buffer assumed to be at
// least large enough for a single element of 'v'.
static inline void vect_void_swap_pop(
    vect_void * v,
    size_t index,
    void * elem_tmp
)
{
    vect_void_swap(v, index, --v->len, elem_tmp);
}

// Erases the specified range from 'v'.
void vect_void_range_erase(
    vect_void * v,
    size_t start,
    size_t end_non_incl
);

// Erases the element at 'index'.
static inline void vect_void_erase(vect_void * v, size_t index)
{
    vect_void_range_erase(v, index, index+1);
}

// Erases every element in the specified range for which 'pred' returns true.
void vect_void_range_erase_if(
    vect_void * v,
    size_t start,
    size_t end_non_incl,
    arr_fpred pred,
    void * arg
);

// Like vect_void_range_erase_if() but goes through the whole of 'v'.
static inline void vect_void_erase_if(vect_void * v, arr_fpred pred, void * arg)
{
    vect_void_range_erase_if(v, 0, v->len, pred, arg);
}

// Inserts the data at 'elem' at 'index' in 'v'.
// Returns: a pointer to the inserted element, NULL otherwise.
void * vect_void_insert(vect_void * v, size_t index, const void * elem);

// Like vect_void_insert() but inserts an array.
vect_void * vect_void_insert_n(
    vect_void * v,
    size_t index,
    const void * src,
    size_t num
);

// Generates type safe wrappers.
#define VECT_DEFINE(NAME, TYPE)                                                \
                                                                               \
typedef struct vect_##NAME {                                                   \
    vect_void vect;                                                            \
} vect_##NAME;                                                                 \
                                                                               \
static inline vect_void * vect_##NAME##_get_base(vect_##NAME * v)              \
{                                                                              \
    return (vect_void *)v;                                                     \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_range_set_to(                                 \
    vect_##NAME * v,                                                           \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    const TYPE * key                                                           \
)                                                                              \
{                                                                              \
    vect_void_range_set_to((vect_void *)v, start, end_non_incl,                \
        (const void *)key);                                                    \
}                                                                              \
static inline void vect_##NAME##_set_to(vect_##NAME * v, const TYPE * key)     \
{                                                                              \
    vect_void_set_to((vect_void *)v, (const void *)key);                       \
}                                                                              \
                                                                               \
static inline vect_##NAME vect_##NAME##_range_copy(                            \
    const vect_##NAME * src,                                                   \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    bool * out_ok                                                              \
)                                                                              \
{                                                                              \
    vect_##NAME ret;                                                           \
    ret.vect = vect_void_range_copy(&src->vect, start, end_non_incl, out_ok);  \
    return ret;                                                                \
}                                                                              \
static inline vect_##NAME vect_##NAME##_copy(                                  \
    const vect_##NAME * src,                                                   \
    bool * out_ok                                                              \
)                                                                              \
{                                                                              \
    vect_##NAME ret;                                                           \
    ret.vect = vect_void_copy(&src->vect, out_ok);                             \
    return ret;                                                                \
}                                                                              \
                                                                               \
static inline bool vect_##NAME##_range_is_sorted(                              \
    const vect_##NAME * v,                                                     \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return vect_void_range_is_sorted((vect_void *)v, start, end_non_incl, cmp);\
}                                                                              \
static inline bool vect_##NAME##_is_sorted(                                    \
    const vect_##NAME * v,                                                     \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return vect_void_is_sorted((vect_void *)v, cmp);                           \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_range_sort(                                   \
    vect_##NAME * v,                                                           \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    vect_void_range_sort((vect_void *)v, start, end_non_incl, cmp);            \
}                                                                              \
static inline void vect_##NAME##_sort(vect_##NAME * v, arr_fcomp cmp)          \
{                                                                              \
    vect_void_sort((vect_void *)v, cmp);                                       \
}                                                                              \
                                                                               \
static inline TYPE * vect_##NAME##_range_find(                                 \
    const vect_##NAME * v,                                                     \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (TYPE *)vect_void_range_find((vect_void *)v, start, end_non_incl,   \
        (const void *)key, cmp);                                               \
}                                                                              \
static inline TYPE * vect_##NAME##_find(                                       \
    const vect_##NAME * v,                                                     \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (TYPE *)vect_void_find((vect_void *)v, (const void *)key, cmp);     \
}                                                                              \
                                                                               \
static inline const TYPE * vect_##NAME##_range_bsearch(                        \
    const vect_##NAME * v,                                                     \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (const TYPE *)vect_void_range_bsearch((vect_void *)v,               \
        start, end_non_incl, (const void *)key, cmp);                          \
}                                                                              \
static inline const TYPE * vect_##NAME##_bsearch(                              \
    const vect_##NAME * v,                                                     \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (const TYPE *)vect_void_bsearch((vect_void *)v,                     \
        (const void *)key, cmp);                                               \
}                                                                              \
                                                                               \
static inline const TYPE * vect_##NAME##_range_sfirst_eq(                      \
    const vect_##NAME * v,                                                     \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (const TYPE *)vect_void_range_sfirst_eq((vect_void *)v,             \
        start, end_non_incl, (const void *)key, cmp);                          \
}                                                                              \
static inline const TYPE * vect_##NAME##_sfirst_eq(                            \
    const vect_##NAME * v,                                                     \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (const TYPE *)vect_void_sfirst_eq((vect_void *)v,                   \
        (const void *)key, cmp);                                               \
}                                                                              \
                                                                               \
static inline const TYPE * vect_##NAME##_range_sfirst_geq(                     \
    const vect_##NAME * v,                                                     \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (const TYPE *)vect_void_range_sfirst_geq((vect_void *)v,            \
        start, end_non_incl, (const void *)key, cmp);                          \
}                                                                              \
static inline const TYPE * vect_##NAME##_sfirst_geq(                           \
    const vect_##NAME * v,                                                     \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (const TYPE *)vect_void_sfirst_geq((vect_void *)v,                  \
        (const void *)key, cmp);                                               \
}                                                                              \
                                                                               \
static inline const TYPE * vect_##NAME##_range_sfirst_gt(                      \
    const vect_##NAME * v,                                                     \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (const TYPE *)vect_void_range_sfirst_gt((vect_void *)v,             \
        start, end_non_incl, (const void *)key, cmp);                          \
}                                                                              \
static inline const TYPE * vect_##NAME##_sfirst_gt(                            \
    const vect_##NAME * v,                                                     \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (const TYPE *)vect_void_sfirst_gt((vect_void *)v,                   \
        (const void *)key, cmp);                                               \
}                                                                              \
                                                                               \
static inline bool vect_##NAME##_is_eq(                                        \
    const vect_##NAME * a,                                                     \
    const vect_##NAME * b,                                                     \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return vect_void_is_eq(&a->vect, &b->vect, cmp);                           \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_swap(                                         \
    vect_##NAME * v,                                                           \
    size_t a,                                                                  \
    size_t b                                                                   \
)                                                                              \
{                                                                              \
    TYPE buff;                                                                 \
    vect_void_swap((vect_void *)v, a, b, (void *)(&buff));                     \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_range_reverse(                                \
    vect_##NAME * v,                                                           \
    size_t start,                                                              \
    size_t end_non_incl                                                        \
)                                                                              \
{                                                                              \
    TYPE buff;                                                                 \
    vect_void_range_reverse((vect_void *)v, start, end_non_incl,               \
        (void *)(&buff));                                                      \
}                                                                              \
static inline void vect_##NAME##_reverse(vect_##NAME * v)                      \
{                                                                              \
    TYPE buff;                                                                 \
    vect_void_reverse((vect_void *)v, (void *)(&buff));                        \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_range_apply(                                  \
    vect_##NAME * v,                                                           \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    arr_fapply fn,                                                             \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    vect_void_range_apply((vect_void *)v, start, end_non_incl, fn, arg);       \
}                                                                              \
static inline void vect_##NAME##_apply(                                        \
    vect_##NAME * v,                                                           \
    arr_fapply fn,                                                             \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    vect_void_apply((vect_void *)v, fn, arg);                                  \
}                                                                              \
                                                                               \
static inline TYPE * vect_##NAME##_range_partition(                            \
    vect_##NAME * v,                                                           \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    arr_fpred pred,                                                            \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    TYPE buff;                                                                 \
    return (TYPE *)vect_void_range_partition((vect_void *)v, start,            \
        end_non_incl, pred, arg, (void *)(&buff));                             \
}                                                                              \
static inline TYPE * vect_##NAME##_partition(                                  \
    vect_##NAME * v,                                                           \
    arr_fpred pred,                                                            \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    TYPE buff;                                                                 \
    return (TYPE *)vect_void_partition((vect_void *)v, pred, arg,              \
        (void *)(&buff));                                                      \
}                                                                              \
                                                                               \
static inline vect_##NAME * vect_##NAME##_create_cap(                          \
    vect_##NAME * v,                                                           \
    size_t cap                                                                 \
)                                                                              \
{                                                                              \
    return (vect_##NAME *)vect_void_create_cap((vect_void *)v, sizeof(TYPE),   \
        cap);                                                                  \
}                                                                              \
                                                                               \
static inline vect_##NAME * vect_##NAME##_create(vect_##NAME * v)              \
{                                                                              \
    return (vect_##NAME *)vect_void_create_cap((vect_void *)v, sizeof(TYPE),   \
        32);                                                                   \
}                                                                              \
                                                                               \
static inline vect_##NAME * vect_##NAME##_append(                              \
    vect_##NAME * v,                                                           \
    const TYPE * src,                                                          \
    size_t num                                                                 \
)                                                                              \
{                                                                              \
    return (vect_##NAME *)vect_void_append((vect_void *)v, (const void *)src,  \
        num);                                                                  \
}                                                                              \
                                                                               \
static inline vect_##NAME * vect_##NAME##_create_arr(                          \
    vect_##NAME  * v,                                                          \
    const TYPE * src,                                                          \
    size_t num                                                                 \
)                                                                              \
{                                                                              \
    return (vect_##NAME *)vect_void_create_arr((vect_void *)v, sizeof(TYPE),   \
        (const void *)src, num);                                               \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_destroy(vect_##NAME * v)                      \
{                                                                              \
    vect_void_destroy((vect_void *)v);                                         \
}                                                                              \
                                                                               \
static inline vect_##NAME * vect_##NAME##_resize(vect_##NAME * v, size_t cap)  \
{                                                                              \
    return (vect_##NAME *)vect_void_resize((vect_void *)v, cap);               \
}                                                                              \
                                                                               \
static inline vect_##NAME * vect_##NAME##_shrink_to_fit(vect_##NAME * v)       \
{                                                                              \
    return (vect_##NAME *)vect_void_resize((vect_void *)v, v->vect.len);       \
}                                                                              \
                                                                               \
static inline TYPE * vect_##NAME##_data(const vect_##NAME * v)                 \
{                                                                              \
    return (TYPE *)vect_void_data((vect_void *)v);                             \
}                                                                              \
                                                                               \
static inline size_t vect_##NAME##_len(const vect_##NAME * v)                  \
{                                                                              \
    return vect_void_len((vect_void *)v);                                      \
}                                                                              \
                                                                               \
static inline size_t vect_##NAME##_cap(const vect_##NAME * v)                  \
{                                                                              \
    return vect_void_cap((vect_void *)v);                                      \
}                                                                              \
                                                                               \
static inline size_t vect_##NAME##_elem_size(const vect_##NAME * v)            \
{                                                                              \
    return vect_void_elem_size((vect_void *)v);                                \
}                                                                              \
                                                                               \
static inline bool vect_##NAME##_is_empty(const vect_##NAME * v)               \
{                                                                              \
    return vect_void_is_empty((vect_void *)v);                                 \
}                                                                              \
                                                                               \
static inline size_t vect_##NAME##_last(const vect_##NAME * v)                 \
{                                                                              \
    return vect_void_last((vect_void *)v);                                     \
}                                                                              \
                                                                               \
static inline TYPE * vect_##NAME##_get(const vect_##NAME * v, size_t index)    \
{                                                                              \
    return (TYPE *)vect_void_get((vect_void *)v, index);                       \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_set_len(vect_##NAME * v, size_t len)          \
{                                                                              \
    vect_void_set_len((vect_void *)v, len);                                    \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_cap_zero(vect_##NAME * v)                     \
{                                                                              \
    vect_void_cap_zero((vect_void *)v);                                        \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_len_zero(vect_##NAME * v)                     \
{                                                                              \
    vect_void_len_zero((vect_void *)v);                                        \
}                                                                              \
                                                                               \
static inline size_t vect_##NAME##_distance(                                   \
    const vect_##NAME * v,                                                     \
    const TYPE * ptr                                                           \
)                                                                              \
{                                                                              \
    return vect_void_distance((vect_void *)v, (const void *)ptr);              \
}                                                                              \
                                                                               \
static inline TYPE * vect_##NAME##_write(                                      \
    vect_##NAME * v,                                                           \
    size_t index,                                                              \
    const TYPE * elem                                                          \
)                                                                              \
{                                                                              \
    return (TYPE *)vect_void_write((vect_void *)v, index, (const void *)elem); \
}                                                                              \
                                                                               \
static inline TYPE * vect_##NAME##_push(vect_##NAME * v, const TYPE * elem)    \
{                                                                              \
    return (TYPE *)vect_void_push((vect_void *)v, (const void *)elem);         \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_pop(vect_##NAME * v)                          \
{                                                                              \
    vect_void_pop((vect_void *)v);                                             \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_pop_n(vect_##NAME * v, size_t n)              \
{                                                                              \
    vect_void_pop_n((vect_void *)v, n);                                        \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_reset(vect_##NAME * v)                        \
{                                                                              \
    vect_void_reset((vect_void *)v);                                           \
}                                                                              \
                                                                               \
static inline TYPE * vect_##NAME##_peek(const vect_##NAME * v)                 \
{                                                                              \
    return (TYPE *)vect_void_peek((vect_void *)v);                             \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_swap_pop(                                     \
    vect_##NAME * v,                                                           \
    size_t index                                                               \
)                                                                              \
{                                                                              \
    TYPE buff;                                                                 \
    vect_void_swap_pop((vect_void *)v, index, (void *)(&buff));                \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_range_erase(                                  \
    vect_##NAME * v,                                                           \
    size_t start,                                                              \
    size_t end_non_incl                                                        \
)                                                                              \
{                                                                              \
    vect_void_range_erase((vect_void *)v, start, end_non_incl);                \
}                                                                              \
static inline void vect_##NAME##_erase(vect_##NAME * v, size_t index)          \
{                                                                              \
    vect_void_erase((vect_void *)v, index);                                    \
}                                                                              \
                                                                               \
static inline void vect_##NAME##_range_erase_if(                               \
    vect_##NAME * v,                                                           \
    size_t start,                                                              \
    size_t end_non_incl,                                                       \
    arr_fpred pred,                                                            \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    vect_void_range_erase_if((vect_void *)v, start, end_non_incl, pred, arg);  \
}                                                                              \
static inline void vect_##NAME##_erase_if(                                     \
    vect_##NAME * v,                                                           \
    arr_fpred pred,                                                            \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    vect_void_erase_if((vect_void *)v, pred, arg);                             \
}                                                                              \
                                                                               \
static inline TYPE * vect_##NAME##_insert(                                     \
    vect_##NAME * v,                                                           \
    size_t index,                                                              \
    const TYPE * elem                                                          \
)                                                                              \
{                                                                              \
    return (TYPE *)vect_void_insert((vect_void *)v, index, (const void *)elem);\
}                                                                              \
                                                                               \
static inline vect_##NAME * vect_##NAME##_insert_n(                            \
    vect_##NAME * v,                                                           \
    size_t index,                                                              \
    const TYPE * src,                                                          \
    size_t num                                                                 \
)                                                                              \
{                                                                              \
    return (vect_##NAME *)vect_void_insert_n((vect_void *)v, index,            \
        (const void *)src, num);                                               \
} typedef TYPE vect_##NAME##_dont_forget_the_semi
#endif
