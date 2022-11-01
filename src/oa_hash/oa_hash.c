#include "oa_hash.h"
#include "misc/misc.h"

#include <limits.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char byte;

#define NO_INDEX SIZE_MAX

#define taken(n)      (n)
#define vacant(n)     ((n)+1)
#define mod_p2(n, p2) ((n) & ((p2)-1))

static inline void zero_out(oah_void * tbl)
{
	memset(tbl, 0, sizeof(*tbl));
}

static inline size_t int_percent(size_t n, size_t perc)
{
	return perc * n / 100;
}

static inline size_t max_elem_num(size_t tbl_size)
{
	return int_percent(tbl_size, OAH_LOAD_FACT);
}

static oah_void * make_tbl_of_size(
	oah_void * tbl,
	size_t key_size,
	size_t val_size,
	oah_get_hash hash,
	oah_compar cmp,
	size_t tbl_size
)
{
	zero_out(tbl);
	
	if (tbl_size & (tbl_size - 1))
		return NULL;
	
	size_t val_offs = misc_align_to(key_size, val_size);
	size_t kv_pair_size = val_offs + val_size;
		
	
	void * data = malloc(tbl_size * kv_pair_size);
	if (!data)
		return NULL;
		
	if (!bset_create(&tbl->is_taken, tbl_size << 1))
		return NULL;
	
	tbl->data = data;
	tbl->key_size = key_size;
	tbl->val_size = val_size;
	tbl->val_offs = val_offs;
	tbl->kv_pair_size = kv_pair_size;
	tbl->elem_left = max_elem_num(tbl_size);
	tbl->tbl_size = tbl_size;
	tbl->hash = hash;
	tbl->cmp = cmp;
	
	return tbl;
}

oah_void * oah_void_create_sz(
	oah_void * tbl,
	size_t key_size,
	size_t val_size,
	oah_get_hash hash,
	oah_compar cmp,
	size_t elem_num
)
{
	size_t tbl_size = elem_num + int_percent(elem_num, OAH_LOAD_FACT);
	return make_tbl_of_size(tbl,
		key_size, val_size, hash, cmp, misc_geq_p2(tbl_size));
}

void oah_void_destroy(oah_void * tbl)
{
	free(tbl->data);
	bset_destroy(&tbl->is_taken);
	zero_out(tbl);
}

size_t oah_void_entries(oah_void * tbl)
{
	return (max_elem_num(tbl->tbl_size) - tbl->elem_left);
}

const void * oah_void_iterate(
	oah_void * tbl,
	oah_iterator * it,
	void ** out_val
)
{
	byte * kv_pair = (byte *)tbl->data;
	size_t kv_pair_size = tbl->kv_pair_size;
	size_t val_offs = kv_pair_size - tbl->val_size;
	size_t end = tbl->tbl_size;
	bit_set * is_taken = &tbl->is_taken;
	
	for (size_t i = it->it; i < end; ++i)
	{
		if (bset_check(is_taken, i << 1))
		{
			it->it = i+1;
			kv_pair += (i * kv_pair_size);
			*out_val = kv_pair + val_offs; 
			return kv_pair;
		}
	}
	
	return NULL;
}

static byte * find(
	oah_void * tbl,
	const void * key,
	size_t ins_point,
	byte ** pins,
	size_t * out_ind
)
{
	byte * ret = NULL;
	byte * ins = NULL;
	byte * kv_pair = NULL;
	size_t bit = 0;
	
	size_t tbl_size = tbl->tbl_size;
	bit_set * is_taken = &tbl->is_taken;
	oah_compar cmp = tbl->cmp;
	byte * data = (byte *)tbl->data;
	size_t kv_pair_size = tbl->kv_pair_size;
	size_t i = ins_point;
	size_t end = tbl_size;
	
find_loop:
	for (;i < end; ++i)
	{		
		bit = i << 1;
		if (bset_check(is_taken, taken(bit)))
		{
			kv_pair = data + (i * kv_pair_size);
			
			if (cmp(kv_pair, key) != 0)
				continue;
			
			*out_ind = i;
			ret = kv_pair;
			goto done;
		}
		
		if (!ins)
		{
			*out_ind = i;
			ins = data + (i * kv_pair_size);
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

static inline oah_void * tbl_grow(oah_void * dest, oah_void * src)
{
	return make_tbl_of_size(dest,
		src->key_size, src->val_size, src->hash, src->cmp, src->tbl_size << 1);
}

static oah_void * grow_and_reinsert(oah_void * dest, oah_void * tbl)
{
	if (tbl_grow(dest, tbl))
	{
		void * val = NULL;
		const void * key = NULL;
		oah_iterator iter = OAH_ITER_INIT;
		
		while ((key = oah_void_iterate(tbl, &iter, &val)))
			oah_void_insert(dest, key, val);
		
		return dest;
	}
	
	return NULL;
}

oah_void * oah_void_insert(oah_void * tbl, const void * key, const void * val)
{
	byte * pins = NULL;
	size_t ind_ins = 0;
	size_t hash_val = tbl->hash(key);
	byte * found = find(tbl,
		key, mod_p2(hash_val, tbl->tbl_size), &pins, &ind_ins);
	
	size_t key_size = tbl->key_size;
	size_t val_size = tbl->val_size;
	size_t val_offs = tbl->val_offs;
	
	if (!found)
	{
		if (!tbl->elem_left)
		{
			oah_void new_tbl_, * new_tbl = NULL;
			
			new_tbl = grow_and_reinsert(&new_tbl_, tbl);
			if (new_tbl)
			{
				oah_void_destroy(tbl);
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
	
	memcpy(found + val_offs, val, val_size);
	return tbl;
}

void * oah_void_lookup(oah_void * tbl, const void * key)
{
	byte * dummy = NULL;
	size_t dummy2 = 0;
	byte * found = NULL;
	
	if ((found = find(tbl,
		key, mod_p2(tbl->hash(key), tbl->tbl_size), &dummy, &dummy2)))
		found += tbl->val_offs;
	
	return found;
}

oah_void * oah_void_remove(oah_void * tbl, const void * key)
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

oah_void * oah_void_clear(oah_void * tbl)
{
	bset_all_clear(&tbl->is_taken);
	tbl->elem_left = max_elem_num(tbl->tbl_size);
	return tbl;
}

oah_void oah_void_copy(oah_void * tbl, bool * is_ok)
{
	oah_void new_tbl = *tbl;
	*is_ok = false;
	
	new_tbl.is_taken = bset_copy(&tbl->is_taken, is_ok);
	if (is_ok)
	{				
		size_t all_mem = tbl->tbl_size * tbl->kv_pair_size;	
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
