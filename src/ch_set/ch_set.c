#include "ch_set.h"
#include "misc/misc.h"

#include <stdlib.h>
#include <string.h>

#define ptr_sz()        sizeof(void *)
#define mod_p2(n, p2)   ((n) & ((p2)-1))

typedef unsigned char byte;

static byte reserved;
#define CHS_NOT_INIT (&reserved)

// has to  be implemented here
const void * chs_priv_not_init_val(void)
{
	return CHS_NOT_INIT;
}

#define CHH_NO_NEXT  NULL

static inline void zero_out(chs_void * tbl)
{
	memset(tbl, 0, sizeof(*tbl));
}

static void init_tbl_data(byte * data, size_t len, size_t elem_size)
{
	byte * start = data;
	byte * end = start + len * elem_size;
	size_t next_offs = elem_size - ptr_sz();
	
	while (start < end)
	{
		*((void **)(start + next_offs)) = CHS_NOT_INIT;
		start += elem_size;
	}
}

chs_void * chs_void_create_sz_pl(
	chs_void * tbl,
	size_t key_size,
	chs_get_hash hash,
	chs_compar cmp,
	size_t elem_num,
	size_t pool_sz
)
{
	zero_out(tbl);
	
	size_t tbl_size = misc_geq_p2(elem_num);
	
	chs_priv_pool * pool = &tbl->chain_pool;
	if (!chs_priv_pool_create(pool, key_size, pool_sz))
		return NULL;
	
	// struct elem {
	//     key
	//     padding maybe
	//     next
	// };
	
	size_t elem_size = chs_priv_pool_elem_size(pool);
	byte * data = (byte *)malloc(tbl_size * elem_size);
	if (!data)
	{
		chs_priv_pool_destroy(pool);
		return NULL;
	}
	
	init_tbl_data(data, tbl_size, elem_size);
	
	tbl->data = data;
	tbl->key_size = key_size;
	tbl->elems = 0;
	tbl->tbl_size = tbl_size;
	tbl->hash = hash;
	tbl->cmp = cmp;
		
	return tbl;
}

void chs_void_destroy(chs_void * tbl)
{
	free(tbl->data);
	chs_priv_pool_destroy(&tbl->chain_pool);
	zero_out(tbl);
}

static void * find_key(chs_void * tbl, const void * key, size_t bucket)
{
	chs_priv_pool * pool = &tbl->chain_pool;
	chs_compar cmp = tbl->cmp;
	
	byte * elem = tbl->data + (bucket * chs_priv_pool_elem_size(pool));
	if (CHS_NOT_INIT != chs_priv_pool_next_get(pool, elem))
	{
		do
		{	
			if (cmp(elem, key) == 0)
				return elem;
				
		} while ((elem = (byte *)chs_priv_pool_next_get(pool, elem)));
	}

	return NULL;
}

const void * chs_void_lookup(chs_void * tbl, const void * key)
{
	return find_key(tbl, key, mod_p2(tbl->hash(key), tbl->tbl_size));
}

static chs_void * grow_tbl(chs_void * tbl)
{
	chs_priv_pool * pool = &tbl->chain_pool;
	
	size_t new_size = tbl->tbl_size * 2;
	size_t elem_size = chs_priv_pool_elem_size(pool);
	
	byte * new_data = (byte *)malloc(new_size * elem_size);
	if (!new_data)
		return NULL;
	
	init_tbl_data(new_data, new_size, elem_size);
	
	byte * old_data = tbl->data;
	byte * start = old_data;
	tbl->data = new_data;
	
	byte * end = start + tbl->tbl_size * elem_size;
	tbl->tbl_size = new_size;
	
	byte * next = CHS_NOT_INIT;
	
	tbl->elems = 0;
	while (start < end)
	{
		next = (byte *)chs_priv_pool_next_get(pool, start);
		
		if (next != CHS_NOT_INIT)
		{
			byte * elem = start;
			while (1)
			{
				chs_void_insert(tbl, elem);
				
				if (next != CHH_NO_NEXT)
				{
					elem = next;
					next = (byte *)chs_priv_pool_next_get(pool, elem);
					chs_priv_pool_put_back(pool, elem);
				}
				else
					break;
			}
		}
		
		start += elem_size;
	}
	
	free(old_data);
	return tbl;
}

chs_void * chs_void_insert(chs_void * tbl, const void * key)
{
	size_t hash = tbl->hash(key);
	size_t bucket = mod_p2(hash, tbl->tbl_size);
	
	byte * key_spot = (byte *)find_key(tbl, key, bucket);
	
	if (!key_spot)
	{
		if (tbl->tbl_size == tbl->elems)
		{
			if (grow_tbl(tbl))
				bucket = mod_p2(hash, tbl->tbl_size);
			else
				return NULL;
		}
		
		chs_priv_pool * pool = &tbl->chain_pool;
		key_spot = tbl->data + (bucket * chs_priv_pool_elem_size(pool));
		void * next = chs_priv_pool_next_get(pool, key_spot);
		
		if (CHS_NOT_INIT == next)
			chs_priv_pool_next_set(pool, key_spot, CHH_NO_NEXT);
		else
		{
			void * new_elem = chs_priv_pool_get(pool);
			chs_priv_pool_next_set(pool, key_spot, new_elem);
			chs_priv_pool_next_set(pool, new_elem, next);
			key_spot = (byte *)new_elem;
		}
		
		memcpy(key_spot, key, tbl->key_size);
		++tbl->elems;
	}
	
	return tbl;
}

chs_void * chs_void_remove(chs_void * tbl, const void * key)
{
	size_t bucket = mod_p2(tbl->hash(key), tbl->tbl_size);
	
	chs_priv_pool * pool = &tbl->chain_pool;
	byte * elem = tbl->data + (bucket * chs_priv_pool_elem_size(pool));
	void * next = chs_priv_pool_next_get(pool, elem);
	
	if (CHS_NOT_INIT != next)
	{
		chs_compar cmp = tbl->cmp;
		if (cmp(elem, key) == 0)
		{
			if (CHH_NO_NEXT == next)
				chs_priv_pool_next_set(pool, elem, CHS_NOT_INIT);
			else
			{
				memcpy(elem, next, chs_priv_pool_elem_size(pool));
				chs_priv_pool_put_back(pool, next);
			}
			--tbl->elems;
		}
		else
		{
			while (next)
			{
				if (cmp(next, key) == 0)
				{
					chs_priv_pool_next_set(pool, elem,
						chs_priv_pool_next_get(pool, next));
					chs_priv_pool_put_back(pool, next);
					--tbl->elems;
					break;
				}
					
				elem = (byte *)next;
				next = chs_priv_pool_next_get(pool, elem);
			}
		}
	}
	
	return tbl;
}

chs_void * chs_void_clear(chs_void * tbl)
{
	chs_priv_pool * pool = &tbl->chain_pool;
	
	size_t elem_size = chs_priv_pool_elem_size(pool);
	byte * start = tbl->data;
	byte * end = start + tbl->tbl_size * elem_size;
	
	void * chain = NULL;
	void * chain_next = NULL;
	while (start < end)
	{
		chain = chs_priv_pool_next_get(pool, start);
		if (chain != CHS_NOT_INIT)
		{
			while (chain)
			{
				chain_next = chs_priv_pool_next_get(pool, chain);
				chs_priv_pool_put_back(pool, chain);
				chain = chain_next;
			}
			chs_priv_pool_next_set(pool, start, CHS_NOT_INIT);
		}
		start += elem_size;
	}
	
	tbl->elems = 0;
	return tbl;
}

chs_void chs_void_copy(chs_void * tbl, bool * is_ok)
{
	*is_ok = false;
	
	chs_void cpy_tbl;
	
	if (chs_void_create_sz_pl(&cpy_tbl, tbl->key_size, tbl->hash, tbl->cmp,
		tbl->elems, chs_priv_pool_curr_cap(&tbl->chain_pool)))
	{
		chs_iterator iter_, * iter = &iter_;
		chs_iterator_init(iter);
		
		const void * key = NULL;
		while ((key = chs_void_iterate(tbl, iter)))
			chs_void_insert(&cpy_tbl, key);
			
		*is_ok = true;
	}
	
	return cpy_tbl;
}

const void * chs_void_iterate(
	chs_void * tbl,
	chs_iterator * it
)
{
	chs_priv_pool * pool = &tbl->chain_pool;
	
	byte * node = it->node;
	size_t bucket = it->bucket;
	size_t tbl_size = tbl->tbl_size;
	byte * the_tbl = tbl->data;
	size_t tbl_elem_sz = chs_priv_pool_elem_size(pool);
	
traverse_list:
	if (node)
	{
		it->node = (byte *)chs_priv_pool_next_get(pool, node);
		return node;
	}
	
	while (!node && bucket < tbl_size)
	{	
		node = the_tbl + (tbl_elem_sz * bucket);
		++bucket;
		
		if (CHS_NOT_INIT == chs_priv_pool_next_get(pool, node))
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
