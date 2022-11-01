#include "ptr_hash.h"
#include "misc/misc.h"

#include <stdlib.h>
#include <string.h>

#define mod_p2(n, p2)   ((n) & ((p2)-1))

typedef unsigned char byte;

static byte reserved;
#define PTRH_NOT_INIT ((ptrh_node *)(&reserved))

const void * ptrh_priv_not_init_val(void)
{
	return PTRH_NOT_INIT;
}

static inline void zero_out(ptrh_void * tbl)
{
	memset(tbl, 0, sizeof(*tbl));
}

static void init_tbl_data(ptrh_node * data, size_t len)
{
	ptrh_node * start = data;
	ptrh_node * end = start + len;
	while (start < end)
	{
		start->next = PTRH_NOT_INIT;
		++start;
	}
}

ptrh_void * ptrh_void_create_sz_pl(
	ptrh_void * tbl,
	ptrh_get_hash hash,
	ptrh_compar cmp,
	size_t elem_num,
	size_t pool_sz
)
{
	zero_out(tbl);
	size_t tbl_size = misc_geq_p2(elem_num);
	
	ptrh_priv_pool * pool = &tbl->chain_pool;
	if (!ptrh_priv_pool_create(pool, pool_sz))
		return NULL;
		
	ptrh_node * data = (ptrh_node *)malloc(tbl_size * sizeof(*data));
	if (!data)
	{
		ptrh_priv_pool_destroy(pool);
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

void ptrh_void_destroy(ptrh_void * tbl)
{
	free(tbl->data);
	ptrh_priv_pool_destroy(&tbl->chain_pool);
	zero_out(tbl);
}

static void * find_value(
	ptrh_void * tbl,
	const void * key,
	size_t bucket,
	ptrh_node ** out_elem
)
{
	ptrh_node * elem = tbl->data + bucket;
	if (PTRH_NOT_INIT != elem->next)
	{
		ptrh_compar cmp = tbl->cmp;
		do
		{	
			if (cmp(elem->key, key) == 0)
			{
				*out_elem = elem;
				return elem->val;
			}
		} while ((elem = (ptrh_node *)elem->next));
	}
	return NULL;
}

void * ptrh_void_lookup(ptrh_void * tbl, const void * key)
{
	ptrh_node * dummy = NULL;
	return find_value(tbl, key, mod_p2(tbl->hash(key), tbl->tbl_size), &dummy);
}

static ptrh_void * grow_tbl(ptrh_void * tbl)
{
	size_t new_size = tbl->tbl_size * 2;
	
	ptrh_node * new_data = (ptrh_node *)malloc(new_size * sizeof(*new_data));
	if (!new_data)
		return NULL;
		
	init_tbl_data(new_data, new_size);
	
	ptrh_node * old_data = tbl->data;
	ptrh_node * start = old_data;
	tbl->data = new_data;
	
	ptrh_node * end = start + tbl->tbl_size;
	tbl->tbl_size = new_size;
	
	ptrh_node * next = NULL;
	
	tbl->elems = 0;
	while (start < end)
	{
		next = (ptrh_node *)start->next;
		
		if (next != PTRH_NOT_INIT)
		{
			ptrh_priv_pool * pool = &tbl->chain_pool;
			ptrh_node * elem = start;
			while (1)
			{
				ptrh_void_insert(tbl, elem->key, elem->val);
				
				if (next != NULL)
				{
					elem = next;
					next = (ptrh_node *)elem->next;
					ptrh_priv_pool_put_back(pool, elem);
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

ptrh_void * ptrh_void_insert(
	ptrh_void * tbl,
	const void * key,
	const void * val
)
{
	size_t hash = tbl->hash(key);
	size_t bucket = mod_p2(hash, tbl->tbl_size);
	
	ptrh_node * elem = NULL;
	void * value = find_value(tbl, key, bucket, &elem);
	
	if (!value)
	{
		if (tbl->tbl_size == tbl->elems)
		{
			if (grow_tbl(tbl))
				bucket = mod_p2(hash, tbl->tbl_size);
			else
				return NULL;
		}
		
		elem = tbl->data + bucket;
		ptrh_node * elem_next = elem->next;
		
		if (PTRH_NOT_INIT == elem_next)
			elem->next = NULL;
		else
		{
			
			ptrh_node * new_elem = ptrh_priv_pool_get(&tbl->chain_pool);
			elem->next = new_elem;
			new_elem->next = elem_next;
			elem = new_elem;
		}
		
		elem->key = (void *)key;
		++tbl->elems;
	}
	
	elem->val = (void *)val;
	return tbl;
}

ptrh_void * ptrh_void_remove(ptrh_void * tbl, const void * key)
{
	size_t bucket = mod_p2(tbl->hash(key), tbl->tbl_size);
	ptrh_node * elem = tbl->data + bucket;
	ptrh_node * elem_next = (ptrh_node *)elem->next;
	
	if (PTRH_NOT_INIT != elem_next)
	{
		ptrh_compar cmp = tbl->cmp;
		if (cmp(elem->key, key) == 0)
		{
			if (!elem_next)
				elem->next = PTRH_NOT_INIT;
			else
			{
				*elem = *elem_next;
				ptrh_priv_pool_put_back(&tbl->chain_pool, elem_next);
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
					ptrh_priv_pool_put_back(&tbl->chain_pool, elem_next);
					--tbl->elems;
					break;
				}
					
				elem = elem_next;
				elem_next = (ptrh_node *)elem->next;
			}
		}
	}
	
	return tbl;
}

ptrh_void * ptrh_void_clear(ptrh_void * tbl)
{
	ptrh_priv_pool * pool = &tbl->chain_pool;
	ptrh_node * start = tbl->data;
	ptrh_node * end = start + tbl->tbl_size;
	
	ptrh_node * chain = NULL;
	ptrh_node * chain_next = NULL;
	while (start < end)
	{
		if (start->next != PTRH_NOT_INIT)
		{
			chain = start->next;
			while (chain)
			{
				chain_next = chain->next;
				ptrh_priv_pool_put_back(pool, chain);
				chain = chain_next;
			}
			start->next = PTRH_NOT_INIT;
		}
		++start;
	}
	
	tbl->elems = 0;
	return tbl;
}

ptrh_void ptrh_void_copy(ptrh_void * tbl, bool * is_ok)
{
	*is_ok = false;
	
	ptrh_void cpy_tbl;
	
	if (ptrh_void_create_sz_pl(&cpy_tbl, tbl->hash, tbl->cmp, tbl->elems,
		ptrh_priv_pool_curr_cap(&tbl->chain_pool)))
	{
		ptrh_iterator iter_, * iter = &iter_;
		ptrh_iterator_init(iter);
		
		const void * key = NULL;
		void * val = NULL;
		
		while ((key = ptrh_void_iterate(tbl, iter, &val)))
			ptrh_void_insert(&cpy_tbl, key, val);
			
		*is_ok = true;
	}
	
	return cpy_tbl;
}

const void * ptrh_void_iterate(
	ptrh_void * tbl,
	ptrh_iterator * it,
	void ** out_val
)
{
	ptrh_node * node = it->node;
	size_t bucket = it->bucket;
	size_t tbl_size = tbl->tbl_size;
	ptrh_node * the_tbl = tbl->data;
	
	*out_val = NULL;
	
traverse_list:
	if (node)
	{
		it->node = (ptrh_node *)node->next;
		*out_val = node->val;
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
