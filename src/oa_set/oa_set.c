#include "oa_set.h"
#include "misc/misc.h"

#include <limits.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char byte;

#define NO_INDEX SIZE_MAX

#define taken(n)      (n)
#define vacant(n)     ((n)+1)
#define mod_p2(n, p2) ((n) & ((p2)-1))

static inline void zero_out(oas_void * tbl)
{
	memset(tbl, 0, sizeof(*tbl));
}

static inline size_t int_percent(size_t n, size_t perc)
{
	return perc * n / 100;
}

static inline size_t max_elem_num(size_t tbl_size)
{
	return int_percent(tbl_size, OAS_LOAD_FACT);
}

static oas_void * make_tbl_of_size(
	oas_void * tbl,
	size_t key_size,
	oas_get_hash hash,
	oas_compar cmp,
	size_t tbl_size
)
{
	zero_out(tbl);
	
	if (tbl_size & (tbl_size - 1))
		return NULL;
	
	void * data = malloc(tbl_size * key_size);
	if (!data)
		return NULL;
		
	if (!bset_create(&tbl->is_taken, tbl_size << 1))
		return NULL;
	
	tbl->data = data;
	tbl->key_size = key_size;
	tbl->elem_left = max_elem_num(tbl_size);
	tbl->tbl_size = tbl_size;
	tbl->hash = hash;
	tbl->cmp = cmp;
	
	return tbl;
}

oas_void * oas_void_create_sz(
	oas_void * tbl,
	size_t key_size,
	oas_get_hash hash,
	oas_compar cmp,
	size_t elem_num
)
{
	size_t tbl_size = elem_num + int_percent(elem_num, OAS_LOAD_FACT);
	return make_tbl_of_size(tbl, key_size, hash, cmp, misc_geq_p2(tbl_size));
}

void oas_void_destroy(oas_void * tbl)
{
	free(tbl->data);
	bset_destroy(&tbl->is_taken);
	zero_out(tbl);
}

size_t oas_void_entries(oas_void * tbl)
{
	return (max_elem_num(tbl->tbl_size) - tbl->elem_left);
}

const void * oas_void_iterate(
	oas_void * tbl,
	oas_iterator * it
)
{
	byte * key = (byte *)tbl->data;
	size_t key_size = tbl->key_size;
	size_t end = tbl->tbl_size;
	bit_set * is_taken = &tbl->is_taken;
	
	for (size_t i = it->it; i < end; ++i)
	{
		if (bset_check(is_taken, i << 1))
		{
			it->it = i+1;
			key += (i * key_size); 
			return key;
		}
	}
	
	return NULL;
}

static byte * find(
	oas_void * tbl,
	const void * key,
	size_t ins_point,
	byte ** pins,
	size_t * out_ind
)
{
	byte * ret = NULL;
	byte * ins = NULL;
	byte * key_elem = NULL;
	size_t bit = 0;
	
	size_t tbl_size = tbl->tbl_size;
	bit_set * is_taken = &tbl->is_taken;
	oas_compar cmp = tbl->cmp;
	byte * data = (byte *)tbl->data;
	size_t key_size = tbl->key_size;
	size_t i = ins_point;
	size_t end = tbl_size;
	
find_loop:
	for (;i < end; ++i)
	{		
		bit = i << 1;
		if (bset_check(is_taken, taken(bit)))
		{
			key_elem = data + (i * key_size);
			
			if (cmp(key_elem, key) != 0)
				continue;
			
			*out_ind = i;
			ret = key_elem;
			goto done;
		}
		
		if (!ins)
		{
			*out_ind = i;
			ins = data + (i * key_size);
		}
		
		if (!bset_check(is_taken, vacant(bit)))
			goto done;
	}
	
	if (end == tbl_size)
	{
		i = 0;
		end = ins_point;
		goto find_loop;
	}
	
done:
	*pins = ins;
	return ret;
}

static inline oas_void * tbl_grow(oas_void * dest, oas_void * src)
{
	return make_tbl_of_size(dest,
		src->key_size, src->hash, src->cmp, src->tbl_size << 1);
}

static oas_void * grow_and_reinsert(oas_void * dest, oas_void * tbl)
{
	if (tbl_grow(dest, tbl))
	{
		const void * key = NULL;
		oas_iterator iter = OAS_ITER_INIT;
		
		while ((key = oas_void_iterate(tbl, &iter)))
			oas_void_insert(dest, key);
		
		return dest;
	}
	
	return NULL;
}

oas_void * oas_void_insert(oas_void * tbl, const void * key)
{
	byte * pins = NULL;
	size_t ind_ins = 0;
	size_t hash_val = tbl->hash(key);
	byte * found = find(tbl,
		key, mod_p2(hash_val, tbl->tbl_size), &pins, &ind_ins);
	
	size_t key_size = tbl->key_size;
	
	if (!found)
	{
		if (!tbl->elem_left)
		{
			oas_void new_tbl_, * new_tbl = NULL;
			
			new_tbl = grow_and_reinsert(&new_tbl_, tbl);
			if (new_tbl)
			{
				oas_void_destroy(tbl);
				*tbl = *new_tbl;
				
				find(tbl, key, mod_p2(hash_val, tbl->tbl_size),
					&pins, &ind_ins);
			}
			else
				return NULL;
		}
		
		found = pins;
		memcpy(pins, key, key_size);
		
		bit_set * is_taken = &tbl->is_taken;
		size_t bit = ind_ins << 1;
		
		bset_set(is_taken, taken(bit));
		bset_clear(is_taken, vacant(bit));
		--tbl->elem_left;
	}
	
	return tbl;
}

void * oas_void_lookup(oas_void * tbl, const void * key)
{
	byte * dumm = NULL;
	size_t dumm2 = 0;

	return find(tbl, key, mod_p2(tbl->hash(key), tbl->tbl_size), &dumm, &dumm2);
}

oas_void * oas_void_remove(oas_void * tbl, const void * key)
{
	byte * dummy = NULL;
	size_t out_ind = 0;
	byte * found = find(tbl,
		key, mod_p2(tbl->hash(key), tbl->tbl_size), &dummy, &out_ind);
		
	if (found)
	{
		bit_set * is_taken = &tbl->is_taken;
		
		size_t bit = out_ind << 1;
		bset_clear(is_taken, taken(bit));
		bset_set(is_taken, vacant(bit));
		++tbl->elem_left;
	}
	
	return tbl;
}

oas_void * oas_void_clear(oas_void * tbl)
{
	bset_all_clear(&tbl->is_taken);
	tbl->elem_left = max_elem_num(tbl->tbl_size);
	return tbl;
}

oas_void oas_void_copy(oas_void * tbl, bool * is_ok)
{
	oas_void new_tbl = *tbl;
	*is_ok = false;
	
	new_tbl.is_taken = bset_copy(&tbl->is_taken, is_ok);
	if (is_ok)
	{				
		size_t all_mem = tbl->tbl_size * tbl->key_size;	
		void * data = malloc(all_mem);
	
		if (data)
		{
			new_tbl.data = data;
			memcpy(data, tbl->data, all_mem);
		}
		else
		{
			zero_out(&new_tbl);
			*is_ok = false;
		}
	}
	
	return new_tbl;
}
