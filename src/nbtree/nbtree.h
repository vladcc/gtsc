#ifndef NBTREE_H
#define NBTREE_H

#include "misc/misc.h"
#include <stddef.h>

// Module:      nbtree
// Description: Generic binary tree and relevant traversals.
// Author: Vladimir Dinev
// vld.dinev@gmail.com
// 2022-10-05

// Do not use directly.
typedef struct nbtree_void {
    void * root;
    size_t left_offs;
    size_t right_offs;
} nbtree_void;

// NULLs the root, saves the offsets to the left and right child.
static inline void nbtree_void_init(
    nbtree_void * tree,
    size_t left_offs,
    size_t right_offs
)
{
    tree->root = NULL;
    tree->left_offs = left_offs;
    tree->right_offs = right_offs;
}

// Sets the root in 'tree'.
static inline void nbtree_void_set_root(nbtree_void * tree, void * root)
{
    tree->root = root;
}

// Returns: The root.
static inline void * nbtree_void_get_root(nbtree_void * tree)
{
    return tree->root;
}

// Returns: The address of the left child pointer.
static inline void ** nbtree_void_get_pleft(nbtree_void * tree, void * node)
{
    return misc_offset_in(node, tree->left_offs);
}

// Returns: The address of the right child pointer.
static inline void ** nbtree_void_get_pright(nbtree_void * tree, void * node)
{
    return misc_offset_in(node, tree->right_offs);
}

// Returns: The value of the left child pointer.
static inline void * nbtree_void_get_left(nbtree_void * tree, void * node)
{
    return *nbtree_void_get_pleft(tree, node);
}

// Returns: The value of the right child pointer.
static inline void * nbtree_void_get_right(nbtree_void * tree, void * node)
{
    return *nbtree_void_get_pright(tree, node);
}

// Sets the left child of 'node' to 'dest'. 'tree' is needed because of the
// offset values in the struct.
static inline void nbtree_void_set_left(
    nbtree_void * tree,
    void * node,
    void * dest
)
{
    *nbtree_void_get_pleft(tree, node) = dest;
}

// Like nbtree_void_set_left() but for the right child.
static inline void nbtree_void_set_right(
    nbtree_void * tree,
    void * node,
    void * dest
)
{
    *nbtree_void_get_pright(tree, node) = dest;
}

// NULLs the left and right pointers of 'node'.
static inline void nbtree_void_make_leaf(nbtree_void * tree, void * node)
{
    nbtree_void_set_left(tree, node, NULL);
    nbtree_void_set_right(tree, node, NULL);
}

// Callback type.
typedef void (*nbtree_fapply)(void * node, void * arg);

// Basic traversal algorithms follow below. 'pre', 'post', and 'in' stand for
// pre-order, post-order, and in-order. The 'r' in e.g. 'rpre' stands for
// 'reverse'. The 'n', 'l', and 'r' at the end of the function names stand for
// 'node', 'left', and 'right'. That is, the order of node visits because I can
// never remember them.
void nbtree_void_pre_nlr(nbtree_void * tree, nbtree_fapply fn, void * arg);
void nbtree_void_post_lrn(nbtree_void * tree, nbtree_fapply fn, void * arg);
void nbtree_void_in_lnr(nbtree_void * tree, nbtree_fapply fn, void * arg);

void nbtree_void_rpre_nrl(nbtree_void * tree, nbtree_fapply fn, void * arg);
void nbtree_void_rpost_rln(nbtree_void * tree, nbtree_fapply fn, void * arg);
void nbtree_void_rin_rnl(nbtree_void * tree, nbtree_fapply fn, void * arg);


// Generates type safe wrappers.
#define NBTREE_DEFINE(NAME, TYPE)                                              \
                                                                               \
typedef struct nbtree_##NAME {                                                 \
    nbtree_void tree;                                                          \
} nbtree_##NAME;                                                               \
                                                                               \
static inline void nbtree_##NAME##_init(nbtree_##NAME * tree)                  \
{                                                                              \
    nbtree_void_init(&tree->tree, offsetof(TYPE, left), offsetof(TYPE, right));\
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_set_root(nbtree_##NAME * tree, TYPE * root) \
{                                                                              \
    nbtree_void_set_root(&tree->tree, root);                                   \
}                                                                              \
                                                                               \
static inline TYPE * nbtree_##NAME##_get_root(nbtree_##NAME * tree)            \
{                                                                              \
    return (TYPE *)tree->tree.root;                                            \
}                                                                              \
                                                                               \
static inline TYPE * nbtree_##NAME##_get_left(                                 \
    nbtree_##NAME * tree,                                                      \
    TYPE * node                                                                \
)                                                                              \
{                                                                              \
    return (TYPE *)nbtree_void_get_left(&tree->tree, (void *)node);            \
}                                                                              \
                                                                               \
static inline TYPE * nbtree_##NAME##_get_right(                                \
    nbtree_##NAME * tree,                                                      \
    TYPE * node                                                                \
)                                                                              \
{                                                                              \
    return (TYPE *)nbtree_void_get_right(&tree->tree, (void *)node);           \
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_set_left(                                   \
    nbtree_##NAME * tree,                                                      \
    TYPE * node,                                                               \
    TYPE * dest                                                                \
)                                                                              \
{                                                                              \
    nbtree_void_set_left(&tree->tree, (void *)node, (void *)dest);             \
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_set_right(                                  \
    nbtree_##NAME * tree,                                                      \
    TYPE * node,                                                               \
    TYPE * dest                                                                \
)                                                                              \
{                                                                              \
    nbtree_void_set_right(&tree->tree, (void *)node, (void *)dest);            \
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_make_leaf(nbtree_##NAME * tree, TYPE * node)\
{                                                                              \
    nbtree_void_make_leaf(&tree->tree, (void *)node);                          \
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_pre_nlr(                                    \
    nbtree_##NAME * tree,                                                      \
    nbtree_fapply fn,                                                          \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    nbtree_void_pre_nlr(&tree->tree, fn, arg);                                 \
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_post_lrn(                                   \
    nbtree_##NAME * tree,                                                      \
    nbtree_fapply fn,                                                          \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    nbtree_void_post_lrn(&tree->tree, fn, arg);                                \
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_in_lnr(                                     \
    nbtree_##NAME * tree,                                                      \
    nbtree_fapply fn,                                                          \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    nbtree_void_in_lnr(&tree->tree,  fn, arg);                                 \
}                                                                              \
                                                                               \
                                                                               \
static inline void nbtree_##NAME##_rpre_nrl(                                   \
    nbtree_##NAME * tree,                                                      \
    nbtree_fapply fn,                                                          \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    nbtree_void_rpre_nrl(&tree->tree, fn, arg);                                \
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_rpost_rln(                                  \
    nbtree_##NAME * tree,                                                      \
    nbtree_fapply fn,                                                          \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    nbtree_void_rpost_rln(&tree->tree, fn, arg);                               \
}                                                                              \
                                                                               \
static inline void nbtree_##NAME##_rin_rnl(                                    \
    nbtree_##NAME * tree,                                                      \
    nbtree_fapply fn,                                                          \
    void * arg                                                                 \
)                                                                              \
{                                                                              \
    nbtree_void_rin_rnl(&tree->tree, fn, arg);                                 \
} typedef TYPE nbtree_##NAME##_dont_forget_the_semi
#endif
