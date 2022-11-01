#ifndef ARR_ALGO_H
#define ARR_ALGO_H

#include "arr_algo_defn.h"
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

// Module:      arr_algo
// Description: Encapsulates various array algos and operations. Most functions
// work with an arr info struct, which is a generic way to represent an array
// and a range. A type safe wrapper generator macro is provided to avoid all the
// dangers of using the void type impl.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-09-25

// 'data' points to the start of an array.
// 'elem_size' is the size of each array element.
// 'start' is the start of the range you wish to operate on.
// 'end_non_incl' is one past the last element of the range.
typedef struct arr_void_info {
    void * data;
    size_t elem_size;
    size_t start;
    size_t end_non_incl;
} arr_void_info;

// A wrapper which populates the arr_void_info struct pointed to by 'inf'.
// Returns: 'inf'
static inline arr_void_info * arr_void_info_set(
    arr_void_info * inf,
    void * data,
    size_t elem_size,
    size_t start,
    size_t end_non_incl
)
{
    inf->data = data;
    inf->elem_size = elem_size;
    inf->start = start;
    inf->end_non_incl = end_non_incl;
    return inf;
}

// Returns: The distance in array index units between the 'data' member of 'inf'
// and the address of 'result'. 'result' is expected to be a pointer returned by
// some other arr_algo function as the result of an operation on 'inf'.
static inline size_t arr_void_distance(
    const arr_void_info * inf,
    const void * result
)
{
    return (((unsigned char *)result) -
        ((unsigned char *)inf->data)) / inf->elem_size;
}

// Returns: A pointer to the element at 'index' in 'arr'.
static inline void * arr_void_get(
    const void * arr,
    size_t index,
    size_t elem_size
)
{
    return (unsigned char *)arr + index * elem_size;
}

// Copies 'elem_size' number of bytes from 'elem' to 'arr[index]'.
// Returns: 'arr'.
static inline void * arr_void_write(
    void * arr,
    size_t index,
    size_t elem_size,
    const void * elem
)
{
    return memcpy(arr_void_get(arr, index, elem_size), elem, elem_size);
}

// Sets the range of the array specified by 'inf' to the value pointed to by
// 'key'.
void arr_void_set_to(arr_void_info * inf, const void * key);

// Copies the range of the array specified by 'inf' to 'dest'.
void arr_void_copy(arr_void_info * inf, void * dest);

// Moves the range of the array specified by 'inf' to 'dest'.
void arr_void_move(arr_void_info * inf, void * dest);

// Compares 'a' and 'b' using the three way comparison function 'cmp'. 'a' and
// 'b' are equal only if their element sizes are equal, their range lengths are
// equal, and all their elements are equal.
// Returns: True if the elements in the ranges of the arrays specified by 'a'
// and 'b' are equal, false otherwise.
bool arr_void_is_eq(arr_void_info * a, arr_void_info * b, arr_fcomp cmp);

// Returns: True of the range of the array specified by 'inf' is sorted, false
// otherwise.
bool arr_void_is_sorted(arr_void_info * inf, arr_fcomp cmp);

// Sorts the range of the array specified by 'inf'.
// Complexity: O(n) if already sorted, same as libc qsort() otherwise.
void arr_void_sort(arr_void_info * inf, arr_fcomp cmp);

// Partitions the range of the array specified by 'inf' such that all elements
// for which 'pred' returns true appear before all other elements. The
// partitioning is unstable. 'arg' is passed to 'pred'. 'elem_tmp' should point
// to a buffer large enough for a single array element.
// Returns: A pointer to the first element not in the partition, NULL if there
// is no such element.
// Complexity: O(n)
void * arr_void_partition(
    arr_void_info * inf,
    arr_fpred pred,
    void * arg,
    void * elem_tmp
);

// Looks for 'key' in the range of the array specified by 'inf'.
// Returns: A pointer to the first element equal to 'key', NULL if there was
// none.
// Complexity: O(n)
void * arr_void_find(arr_void_info * inf, const void * key, arr_fcomp cmp);

// Like arr_void_find() but performs a binary search. The range of the array
// specified by 'inf' is expected to be sorted.
// Returns: A pointer to an element equal to 'key', NULL if there was none.
// Complexity: O(log n)
void * arr_void_bsearch(arr_void_info * inf, const void * key, arr_fcomp cmp);

// Looks for the first element in the range of the array specified by 'inf'
// which is equal to 'key'. The range is expected to be sorted.
// Returns: A pointer to the element if found, NULL otherwise.
// Complexity: O(log n)
void * arr_void_sfirst_eq(arr_void_info * inf, const void * key, arr_fcomp cmp);

// Looks for the first element in the range of the array specified by 'inf'
// which is greater or equal to 'key'. The range is expected to be sorted.
// Returns: A pointer to the element if found, NULL otherwise.
// Complexity: O(log n)
void * arr_void_sfirst_geq(
    arr_void_info * inf,
    const void * key,
    arr_fcomp cmp
);

// Looks for the first element in the range of the array specified by 'inf'
// which is greater than 'key'. The range is expected to be sorted.
// Returns: A pointer to the element if found, NULL otherwise.
// Complexity: O(log n)
void * arr_void_sfirst_gt(arr_void_info * inf, const void * key, arr_fcomp cmp);


// Swaps the elements at indexes 'a' and 'b' in the array specified by 'inf'.
// The range part of 'inf' is ignored.
void arr_void_swap(arr_void_info * inf, size_t a, size_t b, void * elem_tmp);

// Reverse the range of the array specified by 'inf'. 'elem_tmp' should point
// to a buffer large enough for a single array element.
void arr_void_reverse(arr_void_info * inf, void * elem_tmp);

// Calls 'fn' for every element of the range of the array specified by 'inf'.
// 'arg' is passed to 'fn'.
void arr_void_apply(arr_void_info * inf, arr_fapply fn, void * arg);

// Generates type safe wrappers.
#define ARR_ALGO_DEFINE(NAME, TYPE)                                            \
                                                                               \
typedef struct arr_##NAME##_info {                                             \
    arr_void_info info;                                                        \
} arr_##NAME##_info;                                                           \
                                                                               \
static inline arr_##NAME##_info * arr_##NAME##_info_set(                       \
    arr_##NAME##_info * info,                                                  \
    TYPE * data,                                                               \
    size_t start,                                                              \
    size_t end_non_incl                                                        \
)                                                                              \
{                                                                              \
    arr_void_info_set(&info->info,                                             \
        (void *)data, sizeof(TYPE), start, end_non_incl);                      \
    return info;                                                               \
}                                                                              \
                                                                               \
static inline size_t arr_##NAME##_distance(                                    \
    const arr_##NAME##_info * inf,                                             \
    const TYPE * ptr                                                           \
)                                                                              \
{                                                                              \
    return arr_void_distance(&inf->info, (const void *)ptr);                   \
}                                                                              \
                                                                               \
static inline TYPE * arr_##NAME##_get(const TYPE * arr, size_t index)          \
{                                                                              \
    return (TYPE *)arr_void_get((const void *)arr, index, sizeof(TYPE));       \
}                                                                              \
                                                                               \
static inline TYPE * arr_##NAME##_write(                                       \
    TYPE * arr,                                                                \
    size_t index,                                                              \
    const TYPE * elem                                                          \
)                                                                              \
{                                                                              \
    return (TYPE *)arr_void_write((void *)arr, index, sizeof(TYPE),            \
        (const void *) elem);                                                  \
}                                                                              \
                                                                               \
static inline void arr_##NAME##_set_to(                                        \
    arr_##NAME##_info * inf,                                                   \
    const TYPE * key                                                           \
)                                                                              \
{                                                                              \
    arr_void_set_to(&inf->info, (const void *)key);                            \
}                                                                              \
                                                                               \
static inline void arr_##NAME##_copy(arr_##NAME##_info * inf, TYPE * dest)     \
{                                                                              \
    arr_void_copy(&inf->info, (void *)dest);                                   \
}                                                                              \
                                                                               \
static inline void arr_##NAME##_move(arr_##NAME##_info * inf, TYPE * dest)     \
{                                                                              \
    arr_void_move(&inf->info, (void *)dest);                                   \
}                                                                              \
                                                                               \
static inline bool arr_##NAME##_is_eq(                                         \
    arr_##NAME##_info * a,                                                     \
    arr_##NAME##_info * b,                                                     \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return arr_void_is_eq(&a->info, &b->info, cmp);                            \
}                                                                              \
                                                                               \
static inline bool arr_##NAME##_is_sorted(                                     \
    arr_##NAME##_info * inf,                                                   \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return arr_void_is_sorted(&inf->info, cmp);                                \
}                                                                              \
                                                                               \
static inline void arr_##NAME##_sort(arr_##NAME##_info * inf, arr_fcomp cmp)   \
{                                                                              \
    arr_void_sort(&inf->info, cmp);                                            \
}                                                                              \
                                                                               \
static inline TYPE * arr_##NAME##_partition(                                   \
    arr_##NAME##_info * inf,                                                   \
    arr_fpred pred,                                                            \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    TYPE buff;                                                                 \
    return (TYPE *)arr_void_partition(&inf->info, pred, arg, &buff);           \
}                                                                              \
                                                                               \
static inline TYPE * arr_##NAME##_find(                                        \
    arr_##NAME##_info * inf,                                                   \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (TYPE *)arr_void_find(&inf->info, (const void *)key, cmp);          \
}                                                                              \
                                                                               \
static inline TYPE * arr_##NAME##_bsearch(                                     \
    arr_##NAME##_info * inf,                                                   \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (TYPE *)arr_void_bsearch(&inf->info, (const void *)key, cmp);       \
}                                                                              \
                                                                               \
static inline TYPE * arr_##NAME##_sfirst_eq(                                   \
    arr_##NAME##_info * inf,                                                   \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (TYPE *)arr_void_sfirst_eq(&inf->info, (const void *)key, cmp);     \
}                                                                              \
                                                                               \
static inline TYPE * arr_##NAME##_sfirst_geq(                                  \
    arr_##NAME##_info * inf,                                                   \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (TYPE *)arr_void_sfirst_geq(&inf->info, (const void *)key, cmp);    \
}                                                                              \
                                                                               \
static inline TYPE * arr_##NAME##_sfirst_gt(                                   \
    arr_##NAME##_info * inf,                                                   \
    const TYPE * key,                                                          \
    arr_fcomp cmp                                                              \
)                                                                              \
{                                                                              \
    return (TYPE *)arr_void_sfirst_gt(&inf->info, (const void *)key, cmp);     \
}                                                                              \
                                                                               \
static inline void arr_##NAME##_swap(                                          \
    arr_##NAME##_info * inf,                                                   \
    size_t a,                                                                  \
    size_t b                                                                   \
)                                                                              \
{                                                                              \
    TYPE buff;                                                                 \
    arr_void_swap(&inf->info, a, b, &buff);                                    \
}                                                                              \
                                                                               \
static inline void arr_##NAME##_reverse(arr_##NAME##_info * inf)               \
{                                                                              \
    TYPE buff;                                                                 \
    arr_void_reverse(&inf->info, &buff);                                       \
}                                                                              \
                                                                               \
static inline void arr_##NAME##_apply(                                         \
    arr_##NAME##_info * inf,                                                   \
    arr_fapply fn,                                                             \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    arr_void_apply(&inf->info, fn, arg);                                       \
} typedef TYPE arr_algo_##NAME##_dont_forget_the_semi
#endif
