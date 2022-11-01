#ifndef PTR_HASH_NODE_H
#define PTR_HASH_NODE_H

typedef struct ptrh_node {
    void * key;
    void * val;
    struct ptrh_node * next;
} ptrh_node;

#endif
