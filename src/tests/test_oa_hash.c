#include <stdio.h>
#include "test.h"

#include <string.h>
#include "oa_hash/oa_hash.h"

typedef struct map_node {
	int key;
	double val;
} map_node;

static bool map_node_is_eq(map_node lsh, map_node rhs)
{
	return (lsh.key == rhs.key) && (lsh.val == rhs.val);
}

#include "test_oa_hash_types.h"

#define bit_taken(n)    ((n)*2)
#define bit_vacant(n)   (((n)*2)+1)

#define ins_val_make(x) ((double)((x)*10+2.0))

#define UNUSED(x) (void)(x)

static inline size_t ihash(const unsigned char * key, size_t len)
{
	UNUSED(len);
	return *(int *)key;
}

static size_t get_ihash(const void * elem)
{
	return ihash((const unsigned char *)elem, sizeof(int));
}

static int icmp(const void * k1, const void * k2)
{
    int i1 = *((int *)k1);
    int i2 = *((int *)k2);
    return (i1 > i2) - (i1 < i2);
}

static bool test_oah_void_copy(oah_void * tbl)
{
	oah_void cpy_, * cpy = &cpy_;
	bit_set * cpy_bset = &cpy->is_taken;
	
	bool is_ok = false;
	*cpy = oah_void_copy(tbl, &is_ok);
	
	check(is_ok);
	check(tbl->key_size == cpy->key_size);
	check(tbl->val_size == cpy->val_size);
	check(tbl->val_offs == cpy->val_offs);
	check(tbl->kv_pair_size == cpy->kv_pair_size);
	check(oah_void_entries(tbl) == oah_void_entries(cpy));
	check(tbl->elem_left == cpy->elem_left);
	check(tbl->tbl_size == cpy->tbl_size);
	check(bset_num_bits(&tbl->is_taken) == bset_num_bits(cpy_bset));
	check(bset_num_zeroes(&tbl->is_taken) == bset_num_zeroes(cpy_bset));
	check(bset_is_eq(&tbl->is_taken, cpy_bset));
	check(tbl->hash == cpy->hash);
	check(tbl->cmp == cpy->cmp);
	check(oah_void_is_empty(tbl) == oah_void_is_empty(cpy));
	
	const void * petbl_key = NULL;
	void * petbl_val = NULL;
	const void * pecpy_key = NULL;
	void * pecpy_val = NULL;
	
	oah_iterator it_tbl = OAH_ITER_INIT;
	oah_iterator it_cpy = OAH_ITER_INIT;
	
	while ((petbl_key = oah_void_iterate(tbl, &it_tbl, &petbl_val)))
	{
		pecpy_key = oah_void_iterate(cpy, &it_cpy, &pecpy_val); 
		check(icmp(petbl_key, pecpy_key) == 0);
		check(memcmp(petbl_val, pecpy_val, tbl->val_size) == 0);
	}
	
	check(NULL == oah_void_iterate(cpy, &it_cpy, &petbl_val));
	
	oah_void_destroy(cpy);
	
	return true;
}

static bool test_oah_void_create_destroy(void)
{
	oah_void oah_, * oah = &oah_;
	bit_set * bset = &oah->is_taken;
	
	{
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 5) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);		
		check(8 == oah->tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
		
		oah_void_destroy(oah);
	}
	
	{
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 4) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);		
		check(8 == oah->tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
		
		oah_void_destroy(oah);
	}
	
	{
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 3) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);		
		check(8 == oah->tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
		
		oah_void_destroy(oah);
	}
	
	{
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 2) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);
		check(4 == oah->tbl_size);
		check(bset_num_bits(bset) == 4*2);
		check(bset_num_zeroes(bset) == 4*2);
		check(2 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
		
		oah_void_destroy(oah);
	}
	
	{
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 7) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);
		check(16 == oah->tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
		
		oah_void_destroy(oah);
	}
	
	{
		check(oah_void_create(oah, sizeof(int), sizeof(double), get_ihash, icmp) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);
		check(64 == oah->tbl_size);
		check(bset_num_bits(bset) == 64*2);
		check(bset_num_zeroes(bset) == 64*2);
		check(44 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
		
		oah_void_destroy(oah);
	}
	
	return true;
}

typedef unsigned char byte;
static void toah_set_tbl_to(oah_void * tbl, map_node node)
{
	size_t kv_pair_size = tbl->kv_pair_size;
	size_t val_offs = tbl->val_offs;
	byte * data = (byte *)tbl->data;
	byte * elem = NULL;
	for (size_t i = 0, end = tbl->tbl_size; i < end; ++i)
	{
		elem = data + (kv_pair_size * i);
		*((int *)elem) = node.key;
		elem += val_offs;
		*((double *)(elem)) = node.val;
	}
}

static size_t toah_lookup_to_index(oah_void * tbl, void * looked_up_val)
{
	return ((byte *)looked_up_val - (byte *)tbl->data) / tbl->kv_pair_size;
}

map_node map_elem_to_node(oah_void * tbl, size_t index)
{
	map_node ret;
	byte * elem = (byte *)tbl->data + (index * tbl->kv_pair_size);
	ret.key = *((int *)elem);
	elem += tbl->val_offs;
	ret.val = *((double *)(elem));
	return ret;
}

static bool test_oah_void_insert_lookup_remove_clear_iter(void)
{
	// simple insert, remove
	{
		oah_void oah_, * oah = &oah_;
		bit_set * bset = &oah->is_taken;
	
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 7) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);
		check(16 == oah->tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
	
		
		map_node null_node = {0, ins_val_make(0)};
		map_node expect = null_node;
		oah_void * ptbl = oah;
		size_t max = oah->tbl_size;

		toah_set_tbl_to(oah, null_node);

		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; expect.val = ins_val_make(0);
			check(map_node_is_eq(map_elem_to_node(ptbl, i), expect));
		}
		
		check(oah_void_is_empty(oah));
		check(0 == oah_void_entries(oah));
		
		int insert = 1; double ins_val = ins_val_make(insert);
		check(oah_void_lookup(oah, &insert) == NULL);
		
		size_t ins_ind = 1; // insert % max;
		size_t real_ins_ind = 1; //ins_ind;
		check(1 == ins_ind);
		check(1 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(1 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(1 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		check(oah_void_remove(oah, &insert) == oah);
		check(oah_void_is_empty(oah));
		check(0 == oah_void_entries(oah));
		check(!bset_check(bset, bit_taken(ins_ind)));
		check(bset_check(bset, bit_vacant(ins_ind)));
		
		check(oah_void_remove(oah, &insert) == oah);
		check(oah_void_is_empty(oah));
		check(0 == oah_void_entries(oah));
		check(!bset_check(bset, bit_taken(ins_ind)));
		check(bset_check(bset, bit_vacant(ins_ind)));
		
		oah_void_destroy(oah);
	}
	
	// grow test
	{
		oah_void oah_, * oah = &oah_;
		bit_set * bset = &oah->is_taken;
	
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 2) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);
		check(4 == oah->tbl_size);
		check(bset_num_bits(bset) == 4*2);
		check(bset_num_zeroes(bset) == 4*2);
		check(2 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
		
		check(test_oah_void_copy(oah));
		
		
		map_node node_null = {0, ins_val_make(0)};
		map_node expect = node_null;
		int key = 0;
		oah_void * ptbl = oah;
		size_t max = oah->elem_left;
		
		toah_set_tbl_to(oah, node_null);
		
		for (size_t i = 0; i < max; ++i)
		{	
			expect.key = 0; expect.val = ins_val_make(0);
			check(map_node_is_eq(expect, map_elem_to_node(ptbl, i)));
		}
		
		key = 0;
		check(oah_void_remove(oah, &key) == oah);
		check(oah_void_lookup(oah, &key) == NULL);
		check(!oah_void_has(oah, &key));
		
		for (int i = 0; i < (int)max; ++i)
		{
			double ins_val = ins_val_make(i);
			check(oah_void_insert(oah, &i, &ins_val) == oah);
		}
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 0);
			check(ins_val_make(0) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		check(!oah_void_is_empty(oah));
		check(2 == oah_void_entries(oah));
		
		key = 0;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		check(oah_void_has(oah, &key));
		key = 1;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		check(oah_void_has(oah, &key));
		key = 1232;
		check(oah_void_lookup(oah, &key) == NULL);
		check(!oah_void_has(oah, &key));
		
		int insert = 1232; double ins_val = ins_val_make(insert);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 0);
			check(ins_val_make(0) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1232);
			check(ins_val_make(1232) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 0;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		check(oah_void_has(oah, &key));
		key = 1;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		check(oah_void_has(oah, &key));
		key = 1232;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		check(oah_void_has(oah, &key));
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);
		check(8 == oah->tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(5-3 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		
		check(!oah_void_is_empty(oah));
		check(3 == oah_void_entries(oah));
		
		oah_void_destroy(oah);
	}
	
	// non-wrapping insert, remove, vacancy
	{
		oah_void oah_, * oah = &oah_;
		bit_set * bset = &oah->is_taken;
	
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 7) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);
		check(16 == oah->tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
	
		
		int key = 0;
		map_node node_null = {0, ins_val_make(0)};
		map_node expect = node_null;
		oah_void * ptbl = oah;
		size_t max = oah->tbl_size;
	
		toah_set_tbl_to(oah, node_null);
		
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; expect.val = ins_val_make(0);
			check(map_node_is_eq(expect, map_elem_to_node(ptbl, i)));
		}
		check(oah_void_is_empty(oah));
		check(0 == oah_void_entries(oah));
		
		int insert = 1; double ins_val = ins_val_make(insert);
		size_t ins_ind = 1; // insert % max;
		size_t real_ins_ind = 1; //ins_ind;
		check(1 == ins_ind);
		check(1 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(1 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		insert = 17; ins_val = ins_val_make(insert);
		ins_ind = 1; // insert % max;
		real_ins_ind = ins_ind + 1;
		check(1 == ins_ind);
		check(2 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, ((unsigned char *)oah_void_lookup(oah, &insert) - oah->val_offs)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_void_is_empty(oah));
		check(2 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 2; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = ins_ind + 1; // already taken from above
		check(2 == ins_ind);
		check(3 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_void_is_empty(oah));
		check(3 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 33; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = ins_ind + 3; // already taken from above
		check(1 == ins_ind);
		check(4 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_void_is_empty(oah));
		check(4 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));

		//     1  2   3  4  ...
		// ... 1, 17, 2, 33 ...
		{
			void * out_val;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 17);
			check(ins_val_make(17) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 33);
			check(ins_val_make(33) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 17;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(17));
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &key)) == 2);

		expect.key = key; expect.val = ins_val_make(key);
		check(map_node_is_eq(map_elem_to_node(ptbl, 2), expect)); // next free slot

		check(!oah_void_is_empty(oah));
		check(4 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(2)));
		check(!bset_check(bset, bit_vacant(2)));
		
		check(oah_void_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(2)));
		check(bset_check(bset, bit_vacant(2)));
		
		//     1  2  3  4  ...
		// ... 1, x, 2, 33 ...
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 33);
			check(ins_val_make(33) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_void_lookup(oah, &key) == NULL);
		check(!oah_void_is_empty(oah));
		check(3 == oah_void_entries(oah));
		
		key = 1000;
		check(oah_void_lookup(oah, &key) == NULL);
		
		key = 1;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 33;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		
		check(test_oah_void_copy(oah));
		
		insert = 18; ins_val = ins_val_make(insert);
		ins_ind = 2; // insert % max;
		real_ins_ind = ins_ind;
		check(2 == ins_ind);
		check(2 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(4 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		//     1  2   3  4  ...
		// ... 1, 18, 2, 33 ...
		
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 18);
			check(ins_val_make(18) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 33);
			check(ins_val_make(33) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_void_clear(oah) == oah);
		check(oah_void_is_empty(oah));
		check(0 == oah_void_entries(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		
		oah_void_destroy(oah);
	}
	
	// wrap around
	{
		oah_void oah_, * oah = &oah_;
		bit_set * bset = &oah->is_taken;
	
		check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 7) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(sizeof(double) == oah->val_size);
		check(sizeof(int)*2 == oah->val_offs);
		check(sizeof(double)*2 == oah->kv_pair_size);
		check(16 == oah->tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oah_void_is_empty(oah));
	
		
		int key = 0;
		map_node null_node = {0, ins_val_make(0)};
		map_node expect = null_node;
		oah_void * ptbl = oah;
		size_t max = oah->tbl_size;

		toah_set_tbl_to(oah, null_node);
		
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; expect.val = ins_val_make(0);
			check(map_node_is_eq(expect, map_elem_to_node(ptbl, i)));
		}
		
		check(oah_void_is_empty(oah));
		check(0 == oah_void_entries(oah));
		
		int insert = 14; double ins_val = ins_val_make(insert);
		size_t ins_ind = 14; // insert % max;
		size_t real_ins_ind = 14; //ins_ind;
		check(14 == ins_ind);
		check(14 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(1 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		insert = 15; ins_val = ins_val_make(insert);
		ins_ind = 15; // insert % max;
		real_ins_ind = ins_ind;
		check(15 == ins_ind);
		check(15 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_void_is_empty(oah));
		check(2 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 16; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = ins_ind;
		check(0 == ins_ind);
		check(0 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_void_is_empty(oah));
		check(3 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 31; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = 1; // already taken from above
		check(15 == ins_ind);
		check(1 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_void_is_empty(oah));
		check(4 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));

		// 0   1  ... 14  15
		// 16, 31 ... 14, 15
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 16);
			check(ins_val_make(16) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 31);
			check(ins_val_make(31) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 16;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(16));
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &key)) == 0);

		expect.key = key; expect.val = ins_val_make(key);
		check(map_node_is_eq(map_elem_to_node(ptbl, 0), expect)); // next free slot

		check(!oah_void_is_empty(oah));
		check(4 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(0)));
		check(!bset_check(bset, bit_vacant(0)));
		
		check(oah_void_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(0)));
		check(bset_check(bset, bit_vacant(0)));
		
		// 0  1  ... 14  15
		// x, 31 ... 14, 15
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 31);
			check(ins_val_make(31) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_void_lookup(oah, &key) == NULL);
		check(!oah_void_is_empty(oah));
		check(3 == oah_void_entries(oah));
		
		key = 14;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 31;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		
		check(test_oah_void_copy(oah));
		
		insert = 1; ins_val = ins_val_make(insert);
		ins_ind = 1; // insert % max;
		real_ins_ind = ins_ind+1;
		check(1 == ins_ind);
		check(2 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(4 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		key = 1000;
		check(oah_void_lookup(oah, &key) == NULL);
		
		// 0  1   2 ... 14  15
		// x, 31, 1 ... 14, 15
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 31);
			check(ins_val_make(31) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		insert = 32; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = ins_ind;
		check(0 == ins_ind);
		check(0 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(5 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));		
		
		key = 31;
		check(oah_void_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(1)));
		check(bset_check(bset, bit_vacant(1)));
		
		// 0   1  2 ... 14  15
		// 32, x, 1 ... 14, 15
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 32);
			check(ins_val_make(32) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_void_lookup(oah, &key) == NULL);
		check(!oah_void_is_empty(oah));
		check(4 == oah_void_entries(oah));
		
		key = 32;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 14;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		
		insert = 2; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = 3;
		check(2 == ins_ind);
		check(3 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(5 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));		
		
		// 0   1  2  3 ... 14  15
		// 32, x, 1  2 ... 14, 15
		key = 32;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 14;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, 14, 15
		insert = 13; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = insert;
		check(13 == ins_ind);
		check(13 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(6 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 29; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = 1;
		check(13 == ins_ind);
		check(1 == real_ins_ind);
		check(oah_void_insert(oah, &insert, &ins_val) == oah);
		check(*(double *)oah_void_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(oah, oah_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_void_is_empty(oah));
		check(7 == oah_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		// 0   1   2  3 ... 13  14  15
		// 32, 29, 1  2 ... 13, 14, 15
		key = 32;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 29;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 13;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 14;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 32);
			check(ins_val_make(32) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 29);
			check(ins_val_make(29) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 13);
			check(ins_val_make(13) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 29;
		check(oah_void_remove(oah, &key) == oah);
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, 14, 15
		
		key = 14;
		check(oah_void_remove(oah, &key) == oah);
		check(oah_void_lookup(oah, &key) == NULL);
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, x,  15
		check(oah_void_entries(oah) == 5);
		
		key = 32;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 13;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 32);
			check(ins_val_make(32) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 13);
			check(ins_val_make(13) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 14;
		check(oah_void_remove(oah, &key) == oah);
		check(oah_void_lookup(oah, &key) == NULL);
		
		key = 32;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 13;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*(double *)oah_void_lookup(oah, &key) == ins_val_make(key));
		
		{
			void * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 32);
			check(ins_val_make(32) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 13);
			check(ins_val_make(13) == *(double *)out_val);
			check(*(int *)oah_void_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *(double *)out_val);
			check(oah_void_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_void_clear(oah) == oah);
		check(oah_void_is_empty(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		check(0 == oah_void_entries(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		
		oah_void_destroy(oah);
	}
	
	return true;
}

static bool test_oah_void_overwrite(void)
{
	oah_void oah_, * oah = &oah_;
	bit_set * bset = &oah->is_taken;
	
	check(oah_void_create_sz(oah, sizeof(int), sizeof(double), get_ihash, icmp, 5) == oah);
	
	check(oah->data);
	check(sizeof(int) == oah->key_size);		
	check(8 == oah->tbl_size);
	check(bset_num_bits(bset) == 8*2);
	check(bset_num_zeroes(bset) == 8*2);
	check(5 == oah->elem_left);
	check(get_ihash == oah->hash);
	check(icmp == oah->cmp);
	check(oah_void_is_empty(oah));
	
	int key = 5; double val = 10.3;
	
	check(oah_void_insert(oah, &key, &val) == oah);
	check(*(double *)oah_void_lookup(oah, &key) == 10.3);
	
	val = 20.45;
	check(oah_void_insert(oah, &key, &val) == oah);
	check(*(double *)oah_void_lookup(oah, &key) == 20.45);
	
	check(oah_void_remove(oah, &key) == oah);
	check(oah_void_lookup(oah, &key) == NULL);
	
	val = 30.77;
	check(oah_void_insert(oah, &key, &val) == oah);
	check(*(double *)oah_void_lookup(oah, &key) == 30.77);
	
	oah_void_destroy(oah);
	
	return true;
}

OA_HASH_DEFINE(intd, int, double);

static bool test_oah_intd_copy(oah_intd * tbl)
{
	oah_intd cpy_, * cpy = &cpy_;
	bit_set * cpy_bset = &cpy->tbl.is_taken;
	
	bool is_ok = false;
	*cpy = oah_intd_copy(tbl, &is_ok);
	
	check(is_ok);
	check(tbl->tbl.key_size == cpy->tbl.key_size);
	check(tbl->tbl.val_size == cpy->tbl.val_size);
	check(tbl->tbl.val_offs == cpy->tbl.val_offs);
	check(tbl->tbl.kv_pair_size == cpy->tbl.kv_pair_size);
	check(oah_intd_entries(tbl) == oah_intd_entries(cpy));
	check(tbl->tbl.elem_left == cpy->tbl.elem_left);
	check(tbl->tbl.tbl_size == cpy->tbl.tbl_size);
	check(bset_num_bits(&tbl->tbl.is_taken) == bset_num_bits(cpy_bset));
	check(bset_num_zeroes(&tbl->tbl.is_taken) == bset_num_zeroes(cpy_bset));
	check(bset_is_eq(&tbl->tbl.is_taken, cpy_bset));
	check(tbl->tbl.hash == cpy->tbl.hash);
	check(tbl->tbl.cmp == cpy->tbl.cmp);
	check(oah_intd_is_empty(tbl) == oah_intd_is_empty(cpy));
	
	const int * petbl_key = NULL;
	double * petbl_val = NULL;
	const int * pecpy_key = NULL;
	double * pecpy_val = NULL;
	
	oah_iterator it_tbl = OAH_ITER_INIT;
	oah_iterator it_cpy = OAH_ITER_INIT;
	
	while ((petbl_key = oah_intd_iterate(tbl, &it_tbl, &petbl_val)))
	{
		pecpy_key = oah_intd_iterate(cpy, &it_cpy, &pecpy_val); 
		check(icmp(petbl_key, pecpy_key) == 0);
		check(memcmp(petbl_val, pecpy_val, tbl->tbl.val_size) == 0);
	}
	
	check(NULL == oah_intd_iterate(cpy, &it_cpy, &petbl_val));
	
	oah_intd_destroy(cpy);
	
	return true;
}

static bool test_oah_intd_create_destroy(void)
{
	oah_intd oah_, * oah = &oah_;
	bit_set * bset = &oah->tbl.is_taken;
	
	{
		check(oah_intd_create_sz(oah, get_ihash, icmp, 5) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);		
		check(8 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
		
		oah_intd_destroy(oah);
	}
	
	{
		check(oah_intd_create_sz(oah, get_ihash, icmp, 4) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);		
		check(8 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
		
		oah_intd_destroy(oah);
	}
	
	{
		check(oah_intd_create_sz(oah, get_ihash, icmp, 3) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);		
		check(8 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
		
		oah_intd_destroy(oah);
	}
	
	{
		check(oah_intd_create_sz(oah, get_ihash, icmp, 2) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);
		check(4 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 4*2);
		check(bset_num_zeroes(bset) == 4*2);
		check(2 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
		
		oah_intd_destroy(oah);
	}
	
	{
		check(oah_intd_create_sz(oah, get_ihash, icmp, 7) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);
		check(16 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
		
		oah_intd_destroy(oah);
	}
	
	{
		check(oah_intd_create(oah, get_ihash, icmp) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);
		check(64 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 64*2);
		check(bset_num_zeroes(bset) == 64*2);
		check(44 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
		
		oah_intd_destroy(oah);
	}
	
	return true;
}

static bool test_oah_intd_insert_lookup_remove_clear_iter(void)
{
	// simple insert, remove
	{
		oah_intd oah_, * oah = &oah_;
		bit_set * bset = &oah->tbl.is_taken;
	
		check(oah_intd_create_sz(oah, get_ihash, icmp, 7) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);
		check(16 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
	
		
		map_node null_node = {0, ins_val_make(0)};
		map_node expect = null_node;
		oah_void * ptbl = &oah->tbl;
		size_t max = oah->tbl.tbl_size;

		toah_set_tbl_to(ptbl, null_node);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; expect.val = ins_val_make(0);
			check(map_node_is_eq(expect, map_elem_to_node(ptbl, i)));
		}
		
		check(oah_intd_is_empty(oah));
		check(0 == oah_intd_entries(oah));
		
		int insert = 1; double ins_val = ins_val_make(insert);
		check(oah_intd_lookup(oah, &insert) == NULL);
		
		size_t ins_ind = 1; // insert % max;
		size_t real_ins_ind = 1; //ins_ind;
		check(1 == ins_ind);
		check(1 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(1 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(1 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		check(oah_intd_remove(oah, &insert) == oah);
		check(oah_intd_is_empty(oah));
		check(0 == oah_intd_entries(oah));
		check(!bset_check(bset, bit_taken(ins_ind)));
		check(bset_check(bset, bit_vacant(ins_ind)));
		
		check(oah_intd_remove(oah, &insert) == oah);
		check(oah_intd_is_empty(oah));
		check(0 == oah_intd_entries(oah));
		check(!bset_check(bset, bit_taken(ins_ind)));
		check(bset_check(bset, bit_vacant(ins_ind)));
		
		oah_intd_destroy(oah);
	}
	
	// grow test
	{
		oah_intd oah_, * oah = &oah_;
		bit_set * bset = &oah->tbl.is_taken;
	
		check(oah_intd_create_sz(oah, get_ihash, icmp, 2) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);
		check(4 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 4*2);
		check(bset_num_zeroes(bset) == 4*2);
		check(2 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
		
		check(test_oah_intd_copy(oah));
		
		
		map_node node_null = {0, ins_val_make(0)};
		map_node expect = node_null;
		int key = 0;
		oah_void * ptbl = &oah->tbl;
		size_t max = oah->tbl.elem_left;
	
		toah_set_tbl_to(ptbl, node_null);
		
		for (size_t i = 0; i < max; ++i)
		{	
			expect.key = 0; expect.val = ins_val_make(0);
			check(map_node_is_eq(expect, map_elem_to_node(ptbl, i)));
		}
		
		key = 0;
		check(oah_intd_remove(oah, &key) == oah);
		check(oah_intd_lookup(oah, &key) == NULL);
		check(!oah_intd_has(oah, &key));
		
		for (int i = 0; i < (int)max; ++i)
		{
			double ins_val = ins_val_make(i);
			check(oah_intd_insert(oah, &i, &ins_val) == oah);
		}
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 0);
			check(ins_val_make(0) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		check(!oah_intd_is_empty(oah));
		check(2 == oah_intd_entries(oah));
		
		key = 0;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		check(oah_intd_has(oah, &key));
		key = 1;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		check(oah_intd_has(oah, &key));
		key = 1232;
		check(oah_intd_lookup(oah, &key) == NULL);
		check(!oah_intd_has(oah, &key));
		
		int insert = 1232; double ins_val = ins_val_make(insert);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 0);
			check(ins_val_make(0) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 1232);
			check(ins_val_make(1232) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 0;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		check(oah_intd_has(oah, &key));
		key = 1;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		check(oah_intd_has(oah, &key));
		key = 1232;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		check(oah_intd_has(oah, &key));
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);
		check(8 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(5-3 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		
		check(!oah_intd_is_empty(oah));
		check(3 == oah_intd_entries(oah));
		
		oah_intd_destroy(oah);
	}
	
	// non-wrapping insert, remove, vacancy
	{
		oah_intd oah_, * oah = &oah_;
		bit_set * bset = &oah->tbl.is_taken;
	
		check(oah_intd_create_sz(oah, get_ihash, icmp, 7) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);
		check(16 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
	
		
		int key = 0;
		map_node node_null = {0, ins_val_make(0)};
		map_node expect = node_null;
		oah_void * ptbl = &oah->tbl;
		size_t max = oah->tbl.tbl_size;
		
		toah_set_tbl_to(ptbl, node_null);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; expect.val = ins_val_make(0);
			check(map_node_is_eq(expect, map_elem_to_node(ptbl, i)));
		}
		check(oah_intd_is_empty(oah));
		check(0 == oah_intd_entries(oah));
		
		int insert = 1; double ins_val = ins_val_make(insert);
		size_t ins_ind = 1; // insert % max;
		size_t real_ins_ind = 1; //ins_ind;
		check(1 == ins_ind);
		check(1 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(1 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		insert = 17; ins_val = ins_val_make(insert);
		ins_ind = 1; // insert % max;
		real_ins_ind = ins_ind + 1;
		check(1 == ins_ind);
		check(2 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, ((unsigned char *)oah_intd_lookup(oah, &insert) - oah->tbl.val_offs)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_intd_is_empty(oah));
		check(2 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 2; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = ins_ind + 1; // already taken from above
		check(2 == ins_ind);
		check(3 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_intd_is_empty(oah));
		check(3 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 33; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = ins_ind + 3; // already taken from above
		check(1 == ins_ind);
		check(4 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_intd_is_empty(oah));
		check(4 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));

		//     1  2   3  4  ...
		// ... 1, 17, 2, 33 ...
		{
			double * out_val;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 17);
			check(ins_val_make(17) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 33);
			check(ins_val_make(33) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 17;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(17));
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &key)) == 2);

		expect.key = key; expect.val = ins_val_make(key);
		check(map_node_is_eq(map_elem_to_node(ptbl, 2), expect)); // next free slot

		check(!oah_intd_is_empty(oah));
		check(4 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(2)));
		check(!bset_check(bset, bit_vacant(2)));
		
		check(oah_intd_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(2)));
		check(bset_check(bset, bit_vacant(2)));
		
		//     1  2  3  4  ...
		// ... 1, x, 2, 33 ...
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 33);
			check(ins_val_make(33) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_intd_lookup(oah, &key) == NULL);
		check(!oah_intd_is_empty(oah));
		check(3 == oah_intd_entries(oah));
		
		key = 1000;
		check(oah_intd_lookup(oah, &key) == NULL);
		
		key = 1;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 33;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		
		check(test_oah_intd_copy(oah));
		
		insert = 18; ins_val = ins_val_make(insert);
		ins_ind = 2; // insert % max;
		real_ins_ind = ins_ind;
		check(2 == ins_ind);
		check(2 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(4 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		//     1  2   3  4  ...
		// ... 1, 18, 2, 33 ...
		
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 18);
			check(ins_val_make(18) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 33);
			check(ins_val_make(33) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_intd_clear(oah) == oah);
		check(oah_intd_is_empty(oah));
		check(0 == oah_intd_entries(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		
		oah_intd_destroy(oah);
	}
	
	// wrap around
	{
		oah_intd oah_, * oah = &oah_;
		bit_set * bset = &oah->tbl.is_taken;
	
		check(oah_intd_create_sz(oah, get_ihash, icmp, 7) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(sizeof(double) == oah->tbl.val_size);
		check(sizeof(int)*2 == oah->tbl.val_offs);
		check(sizeof(double)*2 == oah->tbl.kv_pair_size);
		check(16 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oah_intd_is_empty(oah));
	
		
		int key = 0;
		map_node null_node = {0, ins_val_make(0)};
		map_node expect = null_node;
		oah_void * ptbl = &oah->tbl;
		size_t max = oah->tbl.tbl_size;

		toah_set_tbl_to(ptbl, null_node);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; expect.val = ins_val_make(0);
			check(map_node_is_eq(expect, map_elem_to_node(ptbl, i)));
		}
		
		check(oah_intd_is_empty(oah));
		check(0 == oah_intd_entries(oah));
		
		int insert = 14; double ins_val = ins_val_make(insert);
		size_t ins_ind = 14; // insert % max;
		size_t real_ins_ind = 14; //ins_ind;
		check(14 == ins_ind);
		check(14 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(1 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		insert = 15; ins_val = ins_val_make(insert);
		ins_ind = 15; // insert % max;
		real_ins_ind = ins_ind;
		check(15 == ins_ind);
		check(15 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_intd_is_empty(oah));
		check(2 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 16; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = ins_ind;
		check(0 == ins_ind);
		check(0 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_intd_is_empty(oah));
		check(3 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 31; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = 1; // already taken from above
		check(15 == ins_ind);
		check(1 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect)); // next free slot

		check(!oah_intd_is_empty(oah));
		check(4 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));

		// 0   1  ... 14  15
		// 16, 31 ... 14, 15
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 16);
			check(ins_val_make(16) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 31);
			check(ins_val_make(31) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 16;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(16));
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &key)) == 0);

		expect.key = key; expect.val = ins_val_make(key);
		check(map_node_is_eq(map_elem_to_node(ptbl, 0), expect)); // next free slot

		check(!oah_intd_is_empty(oah));
		check(4 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(0)));
		check(!bset_check(bset, bit_vacant(0)));
		
		check(oah_intd_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(0)));
		check(bset_check(bset, bit_vacant(0)));
		
		// 0  1  ... 14  15
		// x, 31 ... 14, 15
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 31);
			check(ins_val_make(31) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_intd_lookup(oah, &key) == NULL);
		check(!oah_intd_is_empty(oah));
		check(3 == oah_intd_entries(oah));
		
		key = 14;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 31;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		
		check(test_oah_intd_copy(oah));
		
		insert = 1; ins_val = ins_val_make(insert);
		ins_ind = 1; // insert % max;
		real_ins_ind = ins_ind+1;
		check(1 == ins_ind);
		check(2 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(4 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		key = 1000;
		check(oah_intd_lookup(oah, &key) == NULL);
		
		// 0  1   2 ... 14  15
		// x, 31, 1 ... 14, 15
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 31);
			check(ins_val_make(31) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		insert = 32; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = ins_ind;
		check(0 == ins_ind);
		check(0 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(5 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));		
		
		key = 31;
		check(oah_intd_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(1)));
		check(bset_check(bset, bit_vacant(1)));
		
		// 0   1  2 ... 14  15
		// 32, x, 1 ... 14, 15
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 32);
			check(ins_val_make(32) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_intd_lookup(oah, &key) == NULL);
		check(!oah_intd_is_empty(oah));
		check(4 == oah_intd_entries(oah));
		
		key = 32;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 14;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		
		insert = 2; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = 3;
		check(2 == ins_ind);
		check(3 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(5 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));		
		
		// 0   1  2  3 ... 14  15
		// 32, x, 1  2 ... 14, 15
		key = 32;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 14;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, 14, 15
		insert = 13; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = insert;
		check(13 == ins_ind);
		check(13 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(6 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 29; ins_val = ins_val_make(insert);
		ins_ind = insert % max;
		real_ins_ind = 1;
		check(13 == ins_ind);
		check(1 == real_ins_ind);
		check(oah_intd_insert(oah, &insert, &ins_val) == oah);
		check(*oah_intd_lookup(oah, &insert) == ins_val);
		check(toah_lookup_to_index(&oah->tbl, oah_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; expect.val = ins_val_make(insert);
		check(map_node_is_eq(map_elem_to_node(ptbl, real_ins_ind), expect));

		check(!oah_intd_is_empty(oah));
		check(7 == oah_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		// 0   1   2  3 ... 13  14  15
		// 32, 29, 1  2 ... 13, 14, 15
		key = 32;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 29;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 13;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 14;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 32);
			check(ins_val_make(32) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 29);
			check(ins_val_make(29) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 13);
			check(ins_val_make(13) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 14);
			check(ins_val_make(14) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 29;
		check(oah_intd_remove(oah, &key) == oah);
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, 14, 15
		
		key = 14;
		check(oah_intd_remove(oah, &key) == oah);
		check(oah_intd_lookup(oah, &key) == NULL);
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, x,  15
		check(oah_intd_entries(oah) == 5);
		
		key = 32;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 13;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 32);
			check(ins_val_make(32) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 13);
			check(ins_val_make(13) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		key = 14;
		check(oah_intd_remove(oah, &key) == oah);
		check(oah_intd_lookup(oah, &key) == NULL);
		
		key = 32;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 1;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 2;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 13;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		key = 15;
		check(*oah_intd_lookup(oah, &key) == ins_val_make(key));
		
		{
			double * out_val = NULL;
			oah_iterator it_ = OAH_ITER_INIT, * it = &it_;
			check(*oah_intd_iterate(oah, it, &out_val) == 32);
			check(ins_val_make(32) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 1);
			check(ins_val_make(1) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 2);
			check(ins_val_make(2) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 13);
			check(ins_val_make(13) == *out_val);
			check(*oah_intd_iterate(oah, it, &out_val) == 15);
			check(ins_val_make(15) == *out_val);
			check(oah_intd_iterate(oah, it, &out_val) == NULL);
		}
		
		check(oah_intd_clear(oah) == oah);
		check(oah_intd_is_empty(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		check(0 == oah_intd_entries(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		
		oah_intd_destroy(oah);
	}
	
	return true;
}

static bool test_oah_intd_overwrite(void)
{
	oah_intd oah_, * oah = &oah_;
	
	check(oah_intd_create_sz(oah, get_ihash, icmp, 5) == oah);
	
	int key = 5;
	double val = 10.3;
	
	check(oah_intd_insert(oah, &key, &val) == oah);
	check(*oah_intd_lookup(oah, &key) == val);
	
	val = 20.45;
	check(oah_intd_insert(oah, &key, &val) == oah);
	check(*oah_intd_lookup(oah, &key) == val);
	
	check(oah_intd_remove(oah, &key) == oah);
	check(oah_intd_lookup(oah, &key) == NULL);
	
	val = 30.77;
	check(oah_intd_insert(oah, &key, &val) == oah);
	check(*oah_intd_lookup(oah, &key) == val);
	
	oah_intd_destroy(oah);
	
	return true;
}

// OA_HASH_DEFINE(foo, int, void *);

static bool test_oah_foo_overwrite(void)
{
	static int foo, bar, baz;
	
	const void * pfoo = &foo;
	const void * pbar = &bar;
	const void * pbaz = &baz;
	
	oah_foo oah_, * oah = &oah_;
	
	check(oah_foo_create_sz(oah, get_ihash, icmp, 5) == oah);
	
	int key = 5;
	check(oah_foo_insert(oah, &key, &pfoo) == oah);
	check(*oah_foo_lookup(oah, &key) == pfoo);
	
	check(oah_foo_insert(oah, &key, &pbar) == oah);
	check(*oah_foo_lookup(oah, &key) == pbar);
	
	check(oah_foo_remove(oah, &key) == oah);
	check(oah_foo_lookup(oah, &key) == NULL);
	
	check(oah_foo_insert(oah, &key, &pbaz) == oah);
	check(*oah_foo_lookup(oah, &key) == pbaz);
	
	oah_foo_destroy(oah);
	
	return true;
}

// OA_HASH_DEFINE(bar, void *, int);

static bool test_oah_bar_overwrite(void)
{
	oah_bar oah_, * oah = &oah_;
	
	check(oah_bar_create_sz(oah, get_ihash, icmp, 5) == oah);
	
	const void * key = (void *)27;
	int val = 5;
	check(oah_bar_insert(oah, &key, &val) == oah);
	check(*oah_bar_lookup(oah, &key) == val);
	
	val = 7;
	check(oah_bar_insert(oah, &key, &val) == oah);
	check(*oah_bar_lookup(oah, &key) == val);
	
	check(oah_bar_remove(oah, &key) == oah);
	check(oah_bar_lookup(oah, &key) == NULL);
	
	val = 8;
	check(oah_bar_insert(oah, &key, &val) == oah);
	check(*oah_bar_lookup(oah, &key) == val);
	
	oah_bar_destroy(oah);
	
	return true;
}

// OA_HASH_DEFINE(baz, void *, void *);

static bool test_oah_baz_overwrite(void)
{
	oah_baz oah_, * oah = &oah_;
	
	check(oah_baz_create_sz(oah, get_ihash, icmp, 5) == oah);
	
	const void * key = (void *)27;
	const void * val = (void *)5;
	check(oah_baz_insert(oah, &key, &val) == oah);
	check(*oah_baz_lookup(oah, &key) == val);
	
	val = (void *)7;
	check(oah_baz_insert(oah, &key, &val) == oah);
	check(*oah_baz_lookup(oah, &key) == val);
	
	check(oah_baz_remove(oah, &key) == oah);
	check(oah_baz_lookup(oah, &key) == NULL);
	
	val = (void *)8;
	check(oah_baz_insert(oah, &key, &val) == oah);
	check(*oah_baz_lookup(oah, &key) == val);
	
	oah_baz_destroy(oah);
	
	return true;
}

static ftest tests[] = {
	test_oah_void_create_destroy,
	test_oah_void_insert_lookup_remove_clear_iter,
	test_oah_void_overwrite,
	test_oah_intd_create_destroy,
	test_oah_intd_insert_lookup_remove_clear_iter,
	test_oah_intd_overwrite,
	test_oah_foo_overwrite,
	test_oah_bar_overwrite,
	test_oah_baz_overwrite,
};

//------------------------------------------------------------------------------

int run_tests(void)
{
    int i, end = sizeof(tests)/sizeof(*tests);

    int passed = 0;
    for (i = 0; i < end; ++i)
        if (tests[i]())
            ++passed;

    if (passed != end)
        putchar('\n');

    int failed = end - passed;
    report(passed, failed);
    return failed;
}
//------------------------------------------------------------------------------

int main(void)
{
	return run_tests();
}
//------------------------------------------------------------------------------
