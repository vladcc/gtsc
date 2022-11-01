#ifndef NSLIST_H
#define NSLIST_H

#include "misc/misc.h"
#include <stddef.h>
#include <stdbool.h>

// Module:      nslist
// Description: Generic stack list. 'Stack list' means that you can only push
// and pop from the front of the list but can also otherwise iterate through it.
// This wouldn't be the case with a strict stack. The goal is simplicity.
// Ironically, the type safe macro generated list is smaller and simpler than
// the void one. This makes it seem like the void implementation is redundant.
// It is needed, however, when you want a list in a some other structure and do
// not know the type.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-05

// Do not use directly.
typedef struct nslist_void {
    void * head;
    size_t next_offs;
} nslist_void;

// NULLs the head and saves the offset to the 'next' pointer.
static inline void nslist_void_init(nslist_void * list, size_t next_offs)
{
    list->head = NULL;
    list->next_offs = next_offs;
}

// Assign to the head pointer.
static inline void nslist_void_set(nslist_void * list, void * head)
{
    list->head = head;
}

// Returns: The address of the 'next' pointer in 'elem'. 'elem' is assumed to
// point to an object of whatever type the list is supposed to hold.
static inline void ** nslist_void_pnext(nslist_void * list, void * elem)
{
    return misc_offset_in(elem, list->next_offs);
}

// Returns: The value of the 'next' pointer in 'elem'.
static inline void * nslist_void_next(nslist_void * list, void * elem)
{
    return *nslist_void_pnext(list, elem);
}

// Links 'elem' as the head of the list.
static inline void nslist_void_push(nslist_void * list, void * elem)
{
    *nslist_void_pnext(list, elem) = list->head;
    list->head = elem;
}

// Unlinks the head.
// Returns: The former head of the list, NULL if the list is empty.
static inline void * nslist_void_pop(nslist_void * list)
{
    void * elem = list->head;
    if (elem)
        list->head = *nslist_void_pnext(list, elem);
    return elem;
}

// Returns: The head of the list.
static inline void * nslist_void_head(nslist_void * list)
{
    return list->head;
}

// Callbacks of this type can be called by the list as long as they return true.
typedef bool (*nslist_void_fapply)(void * elem, void * arg);

// Calls 'fn' for each element in 'list' as long as 'fn' returns true.
static inline void nslist_void_apply(
    nslist_void * list,
    nslist_void_fapply fn,
    void * arg
)
{
    for (void * elem = list->head;
        elem && fn(elem, arg);
        elem = nslist_void_next(list, elem))
        continue;
}

// Generates a type safe list implementation. No offset needs to be remembered,
// however, 'TYPE' is expected to have a 'next' pointer member.
#define NSLIST_DEFINE(NAME, TYPE)                                              \
                                                                               \
typedef struct nslist_##NAME {                                                 \
    TYPE * head;                                                               \
} nslist_##NAME;                                                               \
                                                                               \
static inline void nslist_##NAME##_init(nslist_##NAME * list)                  \
{                                                                              \
    list->head = NULL;                                                         \
}                                                                              \
                                                                               \
static inline void nslist_##NAME##_set(nslist_##NAME * list, TYPE * head)      \
{                                                                              \
    list->head = head;                                                         \
}                                                                              \
                                                                               \
static inline void nslist_##NAME##_push(nslist_##NAME * list, TYPE * data)     \
{                                                                              \
    data->next = list->head;                                                   \
    list->head = data;                                                         \
}                                                                              \
                                                                               \
static inline TYPE * nslist_##NAME##_pop(nslist_##NAME * list)                 \
{                                                                              \
    TYPE * data = list->head;                                                  \
    if (data)                                                                  \
        list->head = data->next;                                               \
    return data;                                                               \
}                                                                              \
                                                                               \
static inline TYPE * nslist_##NAME##_head(nslist_##NAME * list)                \
{                                                                              \
    return list->head;                                                         \
}                                                                              \
                                                                               \
typedef bool (*nslist_##NAME##_fapply)(TYPE * node, void * arg);               \
                                                                               \
static inline void nslist_##NAME##_apply(                                      \
    nslist_##NAME * list,                                                      \
    nslist_##NAME##_fapply fn,                                                 \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    for (TYPE * data = list->head; data && fn(data, arg); data = data->next)   \
        continue;                                                              \
} typedef TYPE nslist_##NAME##_dont_forget_the_semi
#endif
