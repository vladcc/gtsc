#include "ptr_hash_priv.h"

#include <stdlib.h>

static inline void pool_list_push(
	ptrh_priv_node_pool_list * list,
	ptrh_priv_node_pool_node * node
)
{
	node->next = list->head;
	list->head = node;
}

static inline void free_list_set(ptrh_priv_node_list * list, ptrh_node * node)
{	
	list->head = node;
}
static inline void free_list_push(ptrh_priv_node_list * list, ptrh_node * node)
{
	node->next = list->head;
	list->head = node;
}
static inline ptrh_node * free_list_pop(ptrh_priv_node_list * list)
{
	ptrh_node * ret = list->head;
	if (ret)
		list->head = ret->next;
	return ret;
}

static ptrh_priv_pool * allocate_pool(ptrh_priv_pool * pool)
{	
	ptrh_priv_pool * ret = NULL;
	size_t elem_num = pool->elem_num;
	
	ptrh_node * mem = (ptrh_node *)malloc(sizeof(ptrh_node)*elem_num);
	if (mem)
	{
		ptrh_priv_node_pool_node * npl =
		(ptrh_priv_node_pool_node *)malloc(sizeof(ptrh_priv_node_pool_node));
			
		if (npl)
		{
			ptrh_node * first = mem;
			ptrh_node * next = NULL;
			ptrh_node * last = first + elem_num-1;
			
			while (first <= last)
			{
				next = first + 1;
				first->next = next;
				first = next;
			}
			last->next = NULL;
			
			npl->nodes = mem;
			pool_list_push(&pool->pool_list, npl);
			free_list_set(&pool->free_list, mem);
			ret = pool;
		}
		else
			free(mem);
	}
	return ret;
}

ptrh_priv_pool * ptrh_priv_pool_create(ptrh_priv_pool * pool, size_t elem_num)
{
	pool->free_list.head = NULL;
	pool->pool_list.head = NULL;
	pool->elem_num = elem_num;
	return allocate_pool(pool);
}

void ptrh_priv_pool_destroy(ptrh_priv_pool * pool)
{
	ptrh_priv_node_pool_node * npl = pool->pool_list.head;
	ptrh_priv_node_pool_node * next = NULL;
	
	while (npl)
	{
		next = npl->next;
		free(npl->nodes);
		free(npl);
		npl = next;
	}
}

ptrh_node * ptrh_priv_pool_get(ptrh_priv_pool * pool)
{
	ptrh_node * ret = free_list_pop(&pool->free_list);
	if (!ret)
	{
		if (allocate_pool(pool))
			ret = free_list_pop(&pool->free_list);
	}
	return ret;
}

void ptrh_priv_pool_put_back(ptrh_priv_pool * pool, ptrh_node * elem)
{
	free_list_push(&pool->free_list, elem);
}

ptrh_node * ptrh_priv_get_elem_addr(ptrh_priv_pool * pool, size_t elem_num)
{
	size_t count = 0;
	size_t pln_elem_count = pool->elem_num;
	ptrh_priv_node_pool_list * pool_list = &pool->pool_list;
	
	ptrh_priv_node_pool_node * pl = pool_list->head;
	while (pl)
	{
		ptrh_node * pn = pl->nodes;
		for (size_t i = 0; i < pln_elem_count; ++i)
		{
			if (count == elem_num)
				return pn + i;
			++count;
		}
		pl = pl->next;
	}
	
	return NULL;
}
