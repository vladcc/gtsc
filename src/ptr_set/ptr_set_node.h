#ifndef PTR_SET_NODE_H
#define PTR_SET_NODE_H

typedef struct ptrs_node {
    void * key;
    struct ptrs_node * next;
} ptrs_node;

#endif
