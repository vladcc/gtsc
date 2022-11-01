#include "ptr_set.h"
#include "misc/misc.h"

#include <stdlib.h>
#include <string.h>

#define mod_p2(n, p2)   ((n) & ((p2)-1))

typedef unsigned char byte;

static byte reserved;
#define PTRH_NOT_INIT ((ptrs_node *)(&reserved))

const void * ptrs_priv_not_init_val(void)
{
	return PTRH_NOT_INIT;
}

static inline void zero_out(ptrs_void * tbl)
{
	memset(tbl, 0, sizeof(*tbl));
}

static void init_tbl_data(ptrs_node * data, size_t len)
{
	ptrs_node * start = data;
	ptrs_node * end = start + len;
	while (start < end)
	{
		start->next = PTRH_NOT_INIT;
		++start;
	}
}

ptrs_void * ptrs_void_create_sz_pl(
	ptrs_void * tbl,
	ptrs_get_hash hash,
	ptrs_compar cmp,
	size_t elem_num,
	size_t pool_sz
)
{
	zero_out(tbl);
	size_t tbl_size = misc_geq_p2(elem_num);
	
	ptrs_priv_pool * pool = &tbl->chain_pool;
	if (!ptrs_priv_pool_create(pool, pool_sz))
		return NULL;
		
	ptrs_node * data = (ptrs_node *)malloc(tbl_size * sizeof(*data));
	if (!data)
	{
		ptrs_priv_pool_destroy(pool);
		return NULL;
	}
	
	init_tbl_data(data, tbl_size);
	
	tbl->data = data;
	tbl->elems = 0;
	tbl->tbl_size = tbl_size;
	tbl->hash = hash;
	tbl->cmp = cmp;
		
	return tbl;
}

void ptrs_void_destroy(ptrs_void * tbl)
{
	free(tbl->data);
	ptrs_priv_pool_destroy(&tbl->chain_pool);
	zero_out(tbl);
}

static void * find_key(
	ptrs_void * tbl,
	const void * key,
	size_t bucket
)
{
	ptrs_node * elem = tbl->data + bucket;
	if (PTRH_NOT_INIT != elem->next)
	{
		ptrs_compar cmp = tbl->cmp;
		do
		{	
			if (cmp(elem->key, key) == 0)
				return elem->key;
				
		} while ((elem = (ptrs_node *)elem->next));
	}
	return NULL;
}

void * ptrs_void_lookup(ptrs_void * tbl, const void * key)
{
	return find_key(tbl, key, mod_p2(tbl->hash(key), tbl->tbl_size));
}

static ptrs_void * grow_tbl(ptrs_void * tbl)
{
	size_t new_size = tbl->tbl_size * 2;
	
	ptrs_node * new_data = (ptrs_node *)malloc(new_size * sizeof(*new_data));
	if (!new_data)
		return NULL;
		
	init_tbl_data(new_data, new_size);
	
	ptrs_node * old_data = tbl->data;
	ptrs_node * start = old_data;
	tbl->data = new_data;
	
	ptrs_node * end = start + tbl->tbl_size;
	tbl->tbl_size = new_size;
	
	ptrs_node * next = NULL;
	
	tbl->elems = 0;
	while (start < end)
	{
		next = (ptrs_node *)start->next;
		
		if (next != PTRH_NOT_INIT)
		{
			ptrs_priv_pool * pool = &tbl->chain_pool;
			ptrs_node * elem = start;
			while (1)
			{
				ptrs_void_insert(tbl, elem->key);
				
				if (next != NULL)
				{
					elem = next;
					next = (ptrs_node *)elem->next;
					ptrs_priv_pool_put_back(pool, elem);
				}
				else
					break;
			}
		}
		
		++start;
	}
	
	free(old_data);
	return tbl;
}

ptrs_void * ptrs_void_insert(
	ptrs_void * tbl,
	const void * key
)
{
	size_t hash = tbl->hash(key);
	size_t bucket = mod_p2(hash, tbl->tbl_size);
	
	ptrs_node * elem = NULL;
	void * found = find_key(tbl, key, bucket);
	
	if (!found)
	{
		if (tbl->tbl_size == tbl->elems)
		{
			if (grow_tbl(tbl))
				bucket = mod_p2(hash, tbl->tbl_size);
			else
				return NULL;
		}
		
		elem = tbl->data + bucket;
		ptrs_node * elem_next = elem->next;
		
		if (PTRH_NOT_INIT == elem_next)
			elem->next = NULL;
		else
		{
			
			ptrs_node * new_elem = ptrs_priv_pool_get(&tbl->chain_pool);
			elem->next = new_elem;
			new_elem->next = elem_next;
			elem = new_elem;
		}
		
		elem->key = (void *)key;
		++tbl->elems;
	}
	
	return tbl;
}

ptrs_void * ptrs_void_remove(ptrs_void * tbl, const void * key)
{
	size_t bucket = mod_p2(tbl->hash(key), tbl->tbl_size);
	ptrs_node * elem = tbl->data + bucket;
	ptrs_node * elem_next = (ptrs_node *)elem->next;
	
	if (PTRH_NOT_INIT != elem_next)
	{
		ptrs_compar cmp = tbl->cmp;
		if (cmp(elem->key, key) == 0)
		{
			if (!elem_next)
				elem->next = PTRH_NOT_INIT;
			else
			{
				*elem = *elem_next;
				ptrs_priv_pool_put_back(&tbl->chain_pool, elem_next);
			}
			--tbl->elems;
		}
		else
		{
			while (elem_next)
			{
				if (cmp(elem_next->key, key) == 0)
				{
					elem->next = elem_next->next;
					ptrs_priv_pool_put_back(&tbl->chain_pool, elem_next);
					--tbl->elems;
					break;
				}
					
				elem = elem_next;
				elem_next = (ptrs_node *)elem->next;
			}
		}
	}
	
	return tbl;
}

ptrs_void * ptrs_void_clear(ptrs_void * tbl)
{
	ptrs_priv_pool * pool = &tbl->chain_pool;
	ptrs_node * start = tbl->data;
	ptrs_node * end = start + tbl->tbl_size;
	
	ptrs_node * chain = NULL;
	ptrs_node * chain_next = NULL;
	while (start < end)
	{
		if (start->next != PTRH_NOT_INIT)
		{
			chain = start->next;
			while (chain)
			{
				chain_next = chain->next;
				ptrs_priv_pool_put_back(pool, chain);
				chain = chain_next;
			}
			start->next = PTRH_NOT_INIT;
		}
		++start;
	}
	
	tbl->elems = 0;
	return tbl;
}

ptrs_void ptrs_void_copy(ptrs_void * tbl, bool * is_ok)
{
	*is_ok = false;
	
	ptrs_void cpy_tbl;
	
	if (ptrs_void_create_sz_pl(&cpy_tbl, tbl->hash, tbl->cmp, tbl->elems,
		ptrs_priv_pool_curr_cap(&tbl->chain_pool)))
	{
		ptrs_iterator iter_, * iter = &iter_;
		ptrs_iterator_init(iter);
		
		const void * key = NULL;
		
		while ((key = ptrs_void_iterate(tbl, iter)))
			ptrs_void_insert(&cpy_tbl, key);
			
		*is_ok = true;
	}
	
	return cpy_tbl;
}

const void * ptrs_void_iterate(
	ptrs_void * tbl,
	ptrs_iterator * it
)
{
	ptrs_node * node = it->node;
	size_t bucket = it->bucket;
	size_t tbl_size = tbl->tbl_size;
	ptrs_node * the_tbl = tbl->data;
	
traverse_list:
	if (node)
	{
		it->node = (ptrs_node *)node->next;
		return node->key;
	}
	
	while (!node && bucket < tbl_size)
	{	
		node = the_tbl + bucket;
		++bucket;
		
		if (PTRH_NOT_INIT == node->next)
		{
			node = NULL;
			continue;
		}
		else
		{
			it->bucket = bucket;
			goto traverse_list;
		}
	}
	
	return NULL;
}
