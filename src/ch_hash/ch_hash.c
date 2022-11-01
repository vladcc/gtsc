#include "ch_hash.h"
#include "misc/misc.h"

#include <stdlib.h>
#include <string.h>

#define ptr_sz()        sizeof(void *)
#define mod_p2(n, p2)   ((n) & ((p2)-1))

typedef unsigned char byte;

static byte reserved;
#define CHH_NOT_INIT (&reserved)

// has to  be implemented here
const void * chh_priv_not_init_val(void)
{
	return CHH_NOT_INIT;
}

#define CHH_NO_NEXT  NULL

static inline void zero_out(chh_void * tbl)
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
		*((void **)(start + next_offs)) = CHH_NOT_INIT;
		start += elem_size;
	}
}

chh_void * chh_void_create_sz_pl(
	chh_void * tbl,
	size_t key_size,
	size_t val_size,
	chh_get_hash hash,
	chh_compar cmp,
	size_t elem_num,
	size_t pool_sz
)
{
	zero_out(tbl);
	
	size_t tbl_size = misc_geq_p2(elem_num);
	
	size_t val_offs = misc_align_to(key_size, val_size);
	chh_priv_pool * pool = &tbl->chain_pool;
	if (!chh_priv_pool_create(pool, val_offs + val_size, pool_sz))
		return NULL;
	
	// struct elem {
	//     key
	//     padding maybe
	//     val
	//     padding maybe
	//     next
	// };
	
	size_t elem_size = chh_priv_pool_elem_size(pool);
	byte * data = (byte *)malloc(tbl_size * elem_size);
	if (!data)
	{
		chh_priv_pool_destroy(pool);
		return NULL;
	}
	
	init_tbl_data(data, tbl_size, elem_size);
	
	tbl->data = data;
	tbl->key_size = key_size;
	tbl->val_size = val_size;
	tbl->val_offs = val_offs;
	tbl->elems = 0;
	tbl->tbl_size = tbl_size;
	tbl->hash = hash;
	tbl->cmp = cmp;
		
	return tbl;
}

void chh_void_destroy(chh_void * tbl)
{
	free(tbl->data);
	chh_priv_pool_destroy(&tbl->chain_pool);
	zero_out(tbl);
}

static void * find_value(chh_void * tbl, const void * key, size_t bucket)
{
	chh_priv_pool * pool = &tbl->chain_pool;
	chh_compar cmp = tbl->cmp;
	
	byte * elem = tbl->data + (bucket * chh_priv_pool_elem_size(pool));
	if (CHH_NOT_INIT != chh_priv_pool_next_get(pool, elem))
	{
		do
		{	
			if (cmp(elem, key) == 0)
				return (elem + tbl->val_offs);
				
		} while ((elem = (byte *)chh_priv_pool_next_get(pool, elem)));
	}

	return NULL;
}

void * chh_void_lookup(chh_void * tbl, const void * key)
{
	return find_value(tbl, key, mod_p2(tbl->hash(key), tbl->tbl_size));
}

static chh_void * grow_tbl(chh_void * tbl)
{
	chh_priv_pool * pool = &tbl->chain_pool;
	
	size_t new_size = tbl->tbl_size * 2;
	size_t elem_size = chh_priv_pool_elem_size(pool);
	
	byte * new_data = (byte *)malloc(new_size * elem_size);
	if (!new_data)
		return NULL;
	
	init_tbl_data(new_data, new_size, elem_size);
	
	byte * old_data = tbl->data;
	byte * start = old_data;
	tbl->data = new_data;
	
	byte * end = start + tbl->tbl_size * elem_size;
	tbl->tbl_size = new_size;
	
	size_t val_offs = tbl->val_offs;
	byte * next = CHH_NOT_INIT;
	
	tbl->elems = 0;
	while (start < end)
	{
		next = (byte *)chh_priv_pool_next_get(pool, start);
		
		if (next != CHH_NOT_INIT)
		{
			byte * elem = start;
			while (1)
			{
				chh_void_insert(tbl, elem, elem + val_offs);
				
				if (next != CHH_NO_NEXT)
				{
					elem = next;
					next = (byte *)chh_priv_pool_next_get(pool, elem);
					chh_priv_pool_put_back(pool, elem);
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

chh_void * chh_void_insert(chh_void * tbl, const void * key, const void * val)
{
	size_t hash = tbl->hash(key);
	size_t bucket = mod_p2(hash, tbl->tbl_size);
	
	byte * value_spot = (byte *)find_value(tbl, key, bucket);
	
	if (!value_spot)
	{
		if (tbl->tbl_size == tbl->elems)
		{
			if (grow_tbl(tbl))
				bucket = mod_p2(hash, tbl->tbl_size);
			else
				return NULL;
		}
		
		chh_priv_pool * pool = &tbl->chain_pool;
		byte * key_spot = tbl->data + (bucket * chh_priv_pool_elem_size(pool));
		void * next = chh_priv_pool_next_get(pool, key_spot);
		
		if (CHH_NOT_INIT == next)
			chh_priv_pool_next_set(pool, key_spot, CHH_NO_NEXT);
		else
		{
			void * new_elem = chh_priv_pool_get(pool);
			chh_priv_pool_next_set(pool, key_spot, new_elem);
			chh_priv_pool_next_set(pool, new_elem, next);
			key_spot = (byte *)new_elem;
		}
		
		memcpy(key_spot, key, tbl->key_size);
		value_spot = key_spot + tbl->val_offs;
		++tbl->elems;
	}
	
	memcpy(value_spot, val, tbl->val_size);
	return tbl;
}

chh_void * chh_void_remove(chh_void * tbl, const void * key)
{
	size_t bucket = mod_p2(tbl->hash(key), tbl->tbl_size);
	
	chh_priv_pool * pool = &tbl->chain_pool;
	byte * elem = tbl->data + (bucket * chh_priv_pool_elem_size(pool));
	void * next = chh_priv_pool_next_get(pool, elem);
	
	if (CHH_NOT_INIT != next)
	{
		chh_compar cmp = tbl->cmp;
		if (cmp(elem, key) == 0)
		{
			if (CHH_NO_NEXT == next)
				chh_priv_pool_next_set(pool, elem, CHH_NOT_INIT);
			else
			{
				memcpy(elem, next, chh_priv_pool_elem_size(pool));
				chh_priv_pool_put_back(pool, next);
			}
			--tbl->elems;
		}
		else
		{
			while (next)
			{
				if (cmp(next, key) == 0)
				{
					chh_priv_pool_next_set(pool, elem,
						chh_priv_pool_next_get(pool, next));
					chh_priv_pool_put_back(pool, next);
					--tbl->elems;
					break;
				}
					
				elem = (byte *)next;
				next = chh_priv_pool_next_get(pool, elem);
			}
		}
	}
	
	return tbl;
}

chh_void * chh_void_clear(chh_void * tbl)
{
	chh_priv_pool * pool = &tbl->chain_pool;
	
	size_t elem_size = chh_priv_pool_elem_size(pool);
	byte * start = tbl->data;
	byte * end = start + tbl->tbl_size * elem_size;
	
	void * chain = NULL;
	void * chain_next = NULL;
	while (start < end)
	{
		chain = chh_priv_pool_next_get(pool, start);
		if (chain != CHH_NOT_INIT)
		{
			while (chain)
			{
				chain_next = chh_priv_pool_next_get(pool, chain);
				chh_priv_pool_put_back(pool, chain);
				chain = chain_next;
			}
			chh_priv_pool_next_set(pool, start, CHH_NOT_INIT);
		}
		start += elem_size;
	}
	
	tbl->elems = 0;
	return tbl;
}

chh_void chh_void_copy(chh_void * tbl, bool * is_ok)
{
	*is_ok = false;
	
	chh_void cpy_tbl;
	
	if (chh_void_create_sz_pl(&cpy_tbl, tbl->key_size, tbl->val_size, tbl->hash,
		tbl->cmp, tbl->elems, chh_priv_pool_curr_cap(&tbl->chain_pool)))
	{
		chh_iterator iter_, * iter = &iter_;
		chh_iterator_init(iter);
		
		const void * key = NULL;
		void * val = NULL;
		
		while ((key = chh_void_iterate(tbl, iter, &val)))
			chh_void_insert(&cpy_tbl, key, val);
			
		*is_ok = true;
	}
	
	return cpy_tbl;
}

const void * chh_void_iterate(
	chh_void * tbl,
	chh_iterator * it,
	void ** out_val
)
{
	chh_priv_pool * pool = &tbl->chain_pool;
	
	byte * node = it->node;
	size_t bucket = it->bucket;
	size_t tbl_size = tbl->tbl_size;
	byte * the_tbl = tbl->data;
	size_t tbl_elem_sz = chh_priv_pool_elem_size(pool);
	
	*out_val = NULL;
	
traverse_list:
	if (node)
	{
		it->node = (byte *)chh_priv_pool_next_get(pool, node);	
		*out_val = node + tbl->val_offs;
		return node;
	}
	
	while (!node && bucket < tbl_size)
	{	
		node = the_tbl + (tbl_elem_sz * bucket);
		++bucket;
		
		if (CHH_NOT_INIT == chh_priv_pool_next_get(pool, node))
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
