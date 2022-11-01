#include "ptr_set_priv.h"

#include <stdlib.h>

static inline void pool_list_push(
	ptrs_priv_node_pool_list * list,
	ptrs_priv_node_pool_node * node
)
{
	node->next = list->head;
	list->head = node;
}

static inline void free_list_set(ptrs_priv_node_list * list, ptrs_node * node)
{	
	list->head = node;
}
static inline void free_list_push(ptrs_priv_node_list * list, ptrs_node * node)
{
	node->next = list->head;
	list->head = node;
}
static inline ptrs_node * free_list_pop(ptrs_priv_node_list * list)
{
	ptrs_node * ret = list->head;
	if (ret)
		list->head = ret->next;
	return ret;
}

static ptrs_priv_pool * allocate_pool(ptrs_priv_pool * pool)
{	
	ptrs_priv_pool * ret = NULL;
	size_t elem_num = pool->elem_num;
	
	ptrs_node * mem = (ptrs_node *)malloc(sizeof(ptrs_node)*elem_num);
	if (mem)
	{
		ptrs_priv_node_pool_node * npl =
		(ptrs_priv_node_pool_node *)malloc(sizeof(ptrs_priv_node_pool_node));
			
		if (npl)
		{
			ptrs_node * first = mem;
			ptrs_node * next = NULL;
			ptrs_node * last = first + elem_num-1;
			
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

ptrs_priv_pool * ptrs_priv_pool_create(ptrs_priv_pool * pool, size_t elem_num)
{
	pool->free_list.head = NULL;
	pool->pool_list.head = NULL;
	pool->elem_num = elem_num;
	return allocate_pool(pool);
}

void ptrs_priv_pool_destroy(ptrs_priv_pool * pool)
{
	ptrs_priv_node_pool_node * npl = pool->pool_list.head;
	ptrs_priv_node_pool_node * next = NULL;
	
	while (npl)
	{
		next = npl->next;
		free(npl->nodes);
		free(npl);
		npl = next;
	}
}

ptrs_node * ptrs_priv_pool_get(ptrs_priv_pool * pool)
{
	ptrs_node * ret = free_list_pop(&pool->free_list);
	if (!ret)
	{
		if (allocate_pool(pool))
			ret = free_list_pop(&pool->free_list);
	}
	return ret;
}

void ptrs_priv_pool_put_back(ptrs_priv_pool * pool, ptrs_node * elem)
{
	free_list_push(&pool->free_list, elem);
}

ptrs_node * ptrs_priv_get_elem_addr(ptrs_priv_pool * pool, size_t elem_num)
{
	size_t count = 0;
	size_t pln_elem_count = pool->elem_num;
	ptrs_priv_node_pool_list * pool_list = &pool->pool_list;
	
	ptrs_priv_node_pool_node * pl = pool_list->head;
	while (pl)
	{
		ptrs_node * pn = pl->nodes;
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
