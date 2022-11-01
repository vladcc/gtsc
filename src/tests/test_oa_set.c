#include <stdio.h>
#include "test.h"

#include <string.h>
#include "oa_set/oa_set.h"

#include "arr_algo/arr_algo.h"

typedef struct set_node {
	int key;
} set_node;

static bool set_node_is_eq(set_node lsh, set_node rhs)
{
	return (lsh.key == rhs.key);
}

ARR_ALGO_DEFINE(mtype, set_node);

#define bit_taken(n)    ((n)*2)
#define bit_vacant(n)   (((n)*2)+1)

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

static bool test_oas_void_copy(oas_void * tbl)
{
	oas_void cpy_, * cpy = &cpy_;
	bit_set * cpy_bset = &cpy->is_taken;
	
	bool is_ok = false;
	*cpy = oas_void_copy(tbl, &is_ok);
	
	check(is_ok);
	check(tbl->key_size == cpy->key_size);
	check(oas_void_entries(tbl) == oas_void_entries(cpy));
	check(tbl->elem_left == cpy->elem_left);
	check(tbl->tbl_size == cpy->tbl_size);
	check(bset_num_bits(&tbl->is_taken) == bset_num_bits(cpy_bset));
	check(bset_num_zeroes(&tbl->is_taken) == bset_num_zeroes(cpy_bset));
	check(bset_is_eq(&tbl->is_taken, cpy_bset));
	check(tbl->hash == cpy->hash);
	check(tbl->cmp == cpy->cmp);
	check(oas_void_is_empty(tbl) == oas_void_is_empty(cpy));
	
	const void * petbl_key = NULL;
	const void * pecpy_key = NULL;
	
	oas_iterator it_tbl = OAS_ITER_INIT;
	oas_iterator it_cpy = OAS_ITER_INIT;
	
	while ((petbl_key = oas_void_iterate(tbl, &it_tbl)))
	{
		pecpy_key = oas_void_iterate(cpy, &it_cpy); 
		check(icmp(petbl_key, pecpy_key) == 0);
	}
	
	check(NULL == oas_void_iterate(cpy, &it_cpy));
	
	oas_void_destroy(cpy);
	
	return true;
}

static bool test_oas_void_create_destroy(void)
{
	oas_void oas_, * oah = &oas_;
	bit_set * bset = &oah->is_taken;
	
	{
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 5) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);		
		check(8 == oah->tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
		
		oas_void_destroy(oah);
	}
	
	{
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 4) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(8 == oah->tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
		
		oas_void_destroy(oah);
	}
	
	{
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 3) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(8 == oah->tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
		
		oas_void_destroy(oah);
	}
	
	{
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 2) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(4 == oah->tbl_size);
		check(bset_num_bits(bset) == 4*2);
		check(bset_num_zeroes(bset) == 4*2);
		check(2 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
		
		oas_void_destroy(oah);
	}
	
	{
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 7) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(16 == oah->tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
		
		oas_void_destroy(oah);
	}
	
	{
		check(oas_void_create(oah, sizeof(int), get_ihash, icmp) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(64 == oah->tbl_size);
		check(bset_num_bits(bset) == 64*2);
		check(bset_num_zeroes(bset) == 64*2);
		check(44 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
		
		oas_void_destroy(oah);
	}
	
	return true;
}

static bool test_oas_void_insert_lookup_remove_clear_iter(void)
{
	// simple insert, remove
	{
		oas_void oas_, * oah = &oas_;
		bit_set * bset = &oah->is_taken;
	
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 7) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(16 == oah->tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
	
		arr_mtype_info the_tbl_, * the_tbl = &the_tbl_;
		arr_mtype_info_set(the_tbl, (set_node *)oah->data, 0, oah->tbl_size);
		
		set_node null_node = {0};
		set_node expect = null_node;
		set_node * ptbl = (set_node *)oah->data;
		size_t max = oah->tbl_size;
		arr_mtype_set_to(the_tbl, &null_node);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; 
			check(set_node_is_eq(expect, *arr_mtype_get(ptbl, i)));
		}
		
		check(oas_void_is_empty(oah));
		check(0 == oas_void_entries(oah));
		
		int insert = 1; 
		check(oas_void_lookup(oah, &insert) == NULL);
		
		size_t ins_ind = 1; // insert % max;
		size_t real_ins_ind = 1; //ins_ind;
		check(1 == ins_ind);
		check(1 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(1 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(1 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		check(oas_void_remove(oah, &insert) == oah);
		check(oas_void_is_empty(oah));
		check(0 == oas_void_entries(oah));
		check(!bset_check(bset, bit_taken(ins_ind)));
		check(bset_check(bset, bit_vacant(ins_ind)));
		
		check(oas_void_remove(oah, &insert) == oah);
		check(oas_void_is_empty(oah));
		check(0 == oas_void_entries(oah));
		check(!bset_check(bset, bit_taken(ins_ind)));
		check(bset_check(bset, bit_vacant(ins_ind)));
		
		oas_void_destroy(oah);
	}
	
	// grow test
	{
		oas_void oas_, * oah = &oas_;
		bit_set * bset = &oah->is_taken;
	
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 2) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(4 == oah->tbl_size);
		check(bset_num_bits(bset) == 4*2);
		check(bset_num_zeroes(bset) == 4*2);
		check(2 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
		
		check(test_oas_void_copy(oah));
		
		arr_mtype_info the_tbl_, * the_tbl = &the_tbl_;
		arr_mtype_info_set(the_tbl, (set_node *)oah->data, 0, oah->tbl_size);
		
		set_node node_null = {0};
		set_node expect = node_null;
		int key = 0;
		set_node * ptbl = (set_node *)oah->data;
		size_t max = oah->elem_left;
		
		arr_mtype_set_to(the_tbl, &node_null);
		
		for (size_t i = 0; i < max; ++i)
		{	
			expect.key = 0; 
			check(set_node_is_eq(expect, *arr_mtype_get(ptbl, i)));
		}
		
		key = 0;
		check(oas_void_remove(oah, &key) == oah);
		check(oas_void_lookup(oah, &key) == NULL);
		check(!oas_void_has(oah, &key));
		
		for (int i = 0; i < (int)max; ++i)
		{
			check(oas_void_insert(oah, &i) == oah);
		}
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 0);
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		check(!oas_void_is_empty(oah));
		check(2 == oas_void_entries(oah));
		
		key = 0;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		check(oas_void_has(oah, &key));
		key = 1;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		check(oas_void_has(oah, &key));
		key = 1232;
		check(oas_void_lookup(oah, &key) == NULL);
		check(!oas_void_has(oah, &key));
		
		int insert = 1232; 
		check(oas_void_insert(oah, &insert) == oah);
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 0);
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 1232);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		key = 0;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		check(oas_void_has(oah, &key));
		key = 1;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		check(oas_void_has(oah, &key));
		key = 1232;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		check(oas_void_has(oah, &key));
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(8 == oah->tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(5-3 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		
		check(!oas_void_is_empty(oah));
		check(3 == oas_void_entries(oah));
		
		oas_void_destroy(oah);
	}
	
	// non-wrapping insert, remove, vacancy
	{
		oas_void oas_, * oah = &oas_;
		bit_set * bset = &oah->is_taken;
	
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 7) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(16 == oah->tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
	
		arr_mtype_info the_tbl_, * the_tbl = &the_tbl_;
		arr_mtype_info_set(the_tbl, (set_node *)oah->data, 0, oah->tbl_size);
		
		int key = 0;
		set_node node_null = {0};
		set_node expect = node_null;
		set_node * ptbl = (set_node *)oah->data;
		size_t max = oah->tbl_size;
		
		arr_mtype_set_to(the_tbl, &node_null);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; 
			check(set_node_is_eq(expect, *arr_mtype_get(ptbl, i)));
		}
		check(oas_void_is_empty(oah));
		check(0 == oas_void_entries(oah));
		
		int insert = 1; 
		size_t ins_ind = 1; // insert % max;
		size_t real_ins_ind = 1; //ins_ind;
		check(1 == ins_ind);
		check(1 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(1 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		insert = 17; 
		ins_ind = 1; // insert % max;
		real_ins_ind = ins_ind + 1;
		check(1 == ins_ind);
		check(2 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)((unsigned char *)oas_void_lookup(oah, &insert))) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_void_is_empty(oah));
		check(2 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 2; 
		ins_ind = insert % max;
		real_ins_ind = ins_ind + 1; // already taken from above
		check(2 == ins_ind);
		check(3 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_void_is_empty(oah));
		check(3 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 33; 
		ins_ind = insert % max;
		real_ins_ind = ins_ind + 3; // already taken from above
		check(1 == ins_ind);
		check(4 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_void_is_empty(oah));
		check(4 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));

		//     1  2   3  4  ...
		// ... 1, 17, 2, 33 ...
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 17);
			check(*(int *)oas_void_iterate(oah, it) == 2);
			check(*(int *)oas_void_iterate(oah, it) == 33);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		key = 17;
		check(*(int *)oas_void_lookup(oah, &key) == 17);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &key)) == 2);

		expect.key = key; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, 2), expect)); // next free slot

		check(!oas_void_is_empty(oah));
		check(4 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(2)));
		check(!bset_check(bset, bit_vacant(2)));
		
		check(oas_void_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(2)));
		check(bset_check(bset, bit_vacant(2)));
		
		//     1  2  3  4  ...
		// ... 1, x, 2, 33 ...
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 2);
			check(*(int *)oas_void_iterate(oah, it) == 33);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		check(oas_void_lookup(oah, &key) == NULL);
		check(!oas_void_is_empty(oah));
		check(3 == oas_void_entries(oah));
		
		key = 1000;
		check(oas_void_lookup(oah, &key) == NULL);
		
		key = 1;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 2;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 33;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		
		check(test_oas_void_copy(oah));
		
		insert = 18; 
		ins_ind = 2; // insert % max;
		real_ins_ind = ins_ind;
		check(2 == ins_ind);
		check(2 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(4 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		//     1  2   3  4  ...
		// ... 1, 18, 2, 33 ...
		
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 18);
			check(*(int *)oas_void_iterate(oah, it) == 2);
			check(*(int *)oas_void_iterate(oah, it) == 33);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		check(oas_void_clear(oah) == oah);
		check(oas_void_is_empty(oah));
		check(0 == oas_void_entries(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		
		oas_void_destroy(oah);
	}
	
	// wrap around
	{
		oas_void oas_, * oah = &oas_;
		bit_set * bset = &oah->is_taken;
	
		check(oas_void_create_sz(oah, sizeof(int), get_ihash, icmp, 7) == oah);
		
		check(oah->data);
		check(sizeof(int) == oah->key_size);
		check(16 == oah->tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->elem_left);
		check(get_ihash == oah->hash);
		check(icmp == oah->cmp);
		check(oas_void_is_empty(oah));
	
		arr_mtype_info the_tbl_, * the_tbl = &the_tbl_;
		arr_mtype_info_set(the_tbl, (set_node *)oah->data, 0, oah->tbl_size);
		
		int key = 0;
		set_node null_node = {0};
		set_node expect = null_node;
		set_node * ptbl = (set_node *)oah->data;
		size_t max = oah->tbl_size;
		arr_mtype_set_to(the_tbl, &null_node);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; 
			check(set_node_is_eq(expect, *arr_mtype_get(ptbl, i)));
		}
		
		check(oas_void_is_empty(oah));
		check(0 == oas_void_entries(oah));
		
		int insert = 14; 
		size_t ins_ind = 14; // insert % max;
		size_t real_ins_ind = 14; //ins_ind;
		check(14 == ins_ind);
		check(14 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(1 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		insert = 15; 
		ins_ind = 15; // insert % max;
		real_ins_ind = ins_ind;
		check(15 == ins_ind);
		check(15 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_void_is_empty(oah));
		check(2 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 16; 
		ins_ind = insert % max;
		real_ins_ind = ins_ind;
		check(0 == ins_ind);
		check(0 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_void_is_empty(oah));
		check(3 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 31; 
		ins_ind = insert % max;
		real_ins_ind = 1; // already taken from above
		check(15 == ins_ind);
		check(1 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_void_is_empty(oah));
		check(4 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));

		// 0   1  ... 14  15
		// 16, 31 ... 14, 15
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 16);
			check(*(int *)oas_void_iterate(oah, it) == 31);
			check(*(int *)oas_void_iterate(oah, it) == 14);
			check(*(int *)oas_void_iterate(oah, it) == 15);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		key = 16;
		check(*(int *)oas_void_lookup(oah, &key) == 16);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &key)) == 0);

		expect.key = key; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, 0), expect)); // next free slot

		check(!oas_void_is_empty(oah));
		check(4 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(0)));
		check(!bset_check(bset, bit_vacant(0)));
		
		check(oas_void_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(0)));
		check(bset_check(bset, bit_vacant(0)));
		
		// 0  1  ... 14  15
		// x, 31 ... 14, 15
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 31);
			check(*(int *)oas_void_iterate(oah, it) == 14);
			check(*(int *)oas_void_iterate(oah, it) == 15);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		check(oas_void_lookup(oah, &key) == NULL);
		check(!oas_void_is_empty(oah));
		check(3 == oas_void_entries(oah));
		
		key = 14;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 15;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 31;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		
		check(test_oas_void_copy(oah));
		
		insert = 1; 
		ins_ind = 1; // insert % max;
		real_ins_ind = ins_ind+1;
		check(1 == ins_ind);
		check(2 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(4 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		key = 1000;
		check(oas_void_lookup(oah, &key) == NULL);
		
		// 0  1   2 ... 14  15
		// x, 31, 1 ... 14, 15
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 31);
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 14);
			check(*(int *)oas_void_iterate(oah, it) == 15);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		insert = 32; 
		ins_ind = insert % max;
		real_ins_ind = ins_ind;
		check(0 == ins_ind);
		check(0 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(5 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));		
		
		key = 31;
		check(oas_void_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(1)));
		check(bset_check(bset, bit_vacant(1)));
		
		// 0   1  2 ... 14  15
		// 32, x, 1 ... 14, 15
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 32);
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 14);
			check(*(int *)oas_void_iterate(oah, it) == 15);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		check(oas_void_lookup(oah, &key) == NULL);
		check(!oas_void_is_empty(oah));
		check(4 == oas_void_entries(oah));
		
		key = 32;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 1;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 14;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 15;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		
		insert = 2; 
		ins_ind = insert % max;
		real_ins_ind = 3;
		check(2 == ins_ind);
		check(3 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(5 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));		
		
		// 0   1  2  3 ... 14  15
		// 32, x, 1  2 ... 14, 15
		key = 32;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 1;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 14;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 15;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 2;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, 14, 15
		insert = 13; 
		ins_ind = insert % max;
		real_ins_ind = insert;
		check(13 == ins_ind);
		check(13 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(6 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 29; 
		ins_ind = insert % max;
		real_ins_ind = 1;
		check(13 == ins_ind);
		check(1 == real_ins_ind);
		check(oas_void_insert(oah, &insert) == oah);
		check(*(int *)oas_void_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_void_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_void_is_empty(oah));
		check(7 == oas_void_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		// 0   1   2  3 ... 13  14  15
		// 32, 29, 1  2 ... 13, 14, 15
		key = 32;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 29;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 1;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 2;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 13;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 14;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 15;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 32);
			check(*(int *)oas_void_iterate(oah, it) == 29);
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 2);
			check(*(int *)oas_void_iterate(oah, it) == 13);
			check(*(int *)oas_void_iterate(oah, it) == 14);
			check(*(int *)oas_void_iterate(oah, it) == 15);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		key = 29;
		check(oas_void_remove(oah, &key) == oah);
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, 14, 15
		
		key = 14;
		check(oas_void_remove(oah, &key) == oah);
		check(oas_void_lookup(oah, &key) == NULL);
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, x,  15
		check(oas_void_entries(oah) == 5);
		
		key = 32;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 1;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 2;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 13;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 15;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 32);
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 2);
			check(*(int *)oas_void_iterate(oah, it) == 13);
			check(*(int *)oas_void_iterate(oah, it) == 15);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		key = 14;
		check(oas_void_remove(oah, &key) == oah);
		check(oas_void_lookup(oah, &key) == NULL);
		
		key = 32;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 1;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 2;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 13;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		key = 15;
		check(*(int *)oas_void_lookup(oah, &key) == key);
		
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*(int *)oas_void_iterate(oah, it) == 32);
			check(*(int *)oas_void_iterate(oah, it) == 1);
			check(*(int *)oas_void_iterate(oah, it) == 2);
			check(*(int *)oas_void_iterate(oah, it) == 13);
			check(*(int *)oas_void_iterate(oah, it) == 15);
			check(oas_void_iterate(oah, it) == NULL);
		}
		
		check(oas_void_clear(oah) == oah);
		check(oas_void_is_empty(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		check(0 == oas_void_entries(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		
		oas_void_destroy(oah);
	}
	
	return true;
}

OA_SET_DEFINE(intd, int);

static bool test_oas_intd_copy(oas_intd * tbl)
{
	oas_intd cpy_, * cpy = &cpy_;
	bit_set * cpy_bset = &cpy->tbl.is_taken;
	
	bool is_ok = false;
	*cpy = oas_intd_copy(tbl, &is_ok);
	
	check(is_ok);
	check(tbl->tbl.key_size == cpy->tbl.key_size);
	check(oas_intd_entries(tbl) == oas_intd_entries(cpy));
	check(tbl->tbl.elem_left == cpy->tbl.elem_left);
	check(tbl->tbl.tbl_size == cpy->tbl.tbl_size);
	check(bset_num_bits(&tbl->tbl.is_taken) == bset_num_bits(cpy_bset));
	check(bset_num_zeroes(&tbl->tbl.is_taken) == bset_num_zeroes(cpy_bset));
	check(bset_is_eq(&tbl->tbl.is_taken, cpy_bset));
	check(tbl->tbl.hash == cpy->tbl.hash);
	check(tbl->tbl.cmp == cpy->tbl.cmp);
	check(oas_intd_is_empty(tbl) == oas_intd_is_empty(cpy));
	
	const int * petbl_key = NULL;
	const int * pecpy_key = NULL;
	
	oas_iterator it_tbl = OAS_ITER_INIT;
	oas_iterator it_cpy = OAS_ITER_INIT;
	
	while ((petbl_key = oas_intd_iterate(tbl, &it_tbl)))
	{
		pecpy_key = oas_intd_iterate(cpy, &it_cpy); 
		check(icmp(petbl_key, pecpy_key) == 0);
	}
	
	check(NULL == oas_intd_iterate(cpy, &it_cpy));
	
	oas_intd_destroy(cpy);
	
	return true;
}

static bool test_oas_intd_create_destroy(void)
{
	oas_intd oas_, * oah = &oas_;
	bit_set * bset = &oah->tbl.is_taken;
	
	{
		check(oas_intd_create_sz(oah, get_ihash, icmp, 5) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);		
		check(8 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
		
		oas_intd_destroy(oah);
	}
	
	{
		check(oas_intd_create_sz(oah, get_ihash, icmp, 4) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(8 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
		
		oas_intd_destroy(oah);
	}
	
	{
		check(oas_intd_create_sz(oah, get_ihash, icmp, 3) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(8 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(bset_num_zeroes(bset) == 8*2);
		check(5 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
		
		oas_intd_destroy(oah);
	}
	
	{
		check(oas_intd_create_sz(oah, get_ihash, icmp, 2) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(4 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 4*2);
		check(bset_num_zeroes(bset) == 4*2);
		check(2 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
		
		oas_intd_destroy(oah);
	}
	
	{
		check(oas_intd_create_sz(oah, get_ihash, icmp, 7) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(16 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
		
		oas_intd_destroy(oah);
	}
	
	{
		check(oas_intd_create(oah, get_ihash, icmp) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(64 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 64*2);
		check(bset_num_zeroes(bset) == 64*2);
		check(44 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
		
		oas_intd_destroy(oah);
	}
	
	return true;
}

static bool test_oas_intd_insert_lookup_remove_clear_iter(void)
{
	// simple insert, remove
	{
		oas_intd oas_, * oah = &oas_;
		bit_set * bset = &oah->tbl.is_taken;
	
		check(oas_intd_create_sz(oah, get_ihash, icmp, 7) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(16 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
	
		arr_mtype_info the_tbl_, * the_tbl = &the_tbl_;
		arr_mtype_info_set(the_tbl, (set_node *)oah->tbl.data, 0, oah->tbl.tbl_size);
		
		set_node null_node = {0};
		set_node expect = null_node;
		set_node * ptbl = (set_node *)oah->tbl.data;
		size_t max = oah->tbl.tbl_size;
		arr_mtype_set_to(the_tbl, &null_node);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; 
			check(set_node_is_eq(expect, *arr_mtype_get(ptbl, i)));
		}
		
		check(oas_intd_is_empty(oah));
		check(0 == oas_intd_entries(oah));
		
		int insert = 1; 
		check(oas_intd_lookup(oah, &insert) == NULL);
		
		size_t ins_ind = 1; // insert % max;
		size_t real_ins_ind = 1; //ins_ind;
		check(1 == ins_ind);
		check(1 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(1 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(1 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		check(oas_intd_remove(oah, &insert) == oah);
		check(oas_intd_is_empty(oah));
		check(0 == oas_intd_entries(oah));
		check(!bset_check(bset, bit_taken(ins_ind)));
		check(bset_check(bset, bit_vacant(ins_ind)));
		
		check(oas_intd_remove(oah, &insert) == oah);
		check(oas_intd_is_empty(oah));
		check(0 == oas_intd_entries(oah));
		check(!bset_check(bset, bit_taken(ins_ind)));
		check(bset_check(bset, bit_vacant(ins_ind)));
		
		oas_intd_destroy(oah);
	}
	
	// grow test
	{
		oas_intd oas_, * oah = &oas_;
		bit_set * bset = &oah->tbl.is_taken;
	
		check(oas_intd_create_sz(oah, get_ihash, icmp, 2) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(4 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 4*2);
		check(bset_num_zeroes(bset) == 4*2);
		check(2 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
		
		check(test_oas_intd_copy(oah));
		
		arr_mtype_info the_tbl_, * the_tbl = &the_tbl_;
		arr_mtype_info_set(the_tbl, (set_node *)oah->tbl.data, 0, oah->tbl.tbl_size);
		
		set_node node_null = {0};
		set_node expect = node_null;
		int key = 0;
		set_node * ptbl = (set_node *)oah->tbl.data;
		size_t max = oah->tbl.elem_left;
		
		arr_mtype_set_to(the_tbl, &node_null);
		
		for (size_t i = 0; i < max; ++i)
		{	
			expect.key = 0; 
			check(set_node_is_eq(expect, *arr_mtype_get(ptbl, i)));
		}
		
		key = 0;
		check(oas_intd_remove(oah, &key) == oah);
		check(oas_intd_lookup(oah, &key) == NULL);
		check(!oas_intd_has(oah, &key));
		
		for (int i = 0; i < (int)max; ++i)
		{
			
			check(oas_intd_insert(oah, &i) == oah);
		}
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 0);
			check(*oas_intd_iterate(oah, it) == 1);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		check(!oas_intd_is_empty(oah));
		check(2 == oas_intd_entries(oah));
		
		key = 0;
		check(*oas_intd_lookup(oah, &key) == key);
		check(oas_intd_has(oah, &key));
		key = 1;
		check(*oas_intd_lookup(oah, &key) == key);
		check(oas_intd_has(oah, &key));
		key = 1232;
		check(oas_intd_lookup(oah, &key) == NULL);
		check(!oas_intd_has(oah, &key));
		
		int insert = 1232; 
		check(oas_intd_insert(oah, &insert) == oah);
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 0);
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 1232);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		key = 0;
		check(*oas_intd_lookup(oah, &key) == key);
		check(oas_intd_has(oah, &key));
		key = 1;
		check(*oas_intd_lookup(oah, &key) == key);
		check(oas_intd_has(oah, &key));
		key = 1232;
		check(*oas_intd_lookup(oah, &key) == key);
		check(oas_intd_has(oah, &key));
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(8 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 8*2);
		check(5-3 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		
		check(!oas_intd_is_empty(oah));
		check(3 == oas_intd_entries(oah));
		
		oas_intd_destroy(oah);
	}
	
	// non-wrapping insert, remove, vacancy
	{
		oas_intd oas_, * oah = &oas_;
		bit_set * bset = &oah->tbl.is_taken;
	
		check(oas_intd_create_sz(oah, get_ihash, icmp, 7) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(16 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
	
		arr_mtype_info the_tbl_, * the_tbl = &the_tbl_;
		arr_mtype_info_set(the_tbl, (set_node *)oah->tbl.data, 0, oah->tbl.tbl_size);
		
		int key = 0;
		set_node node_null = {0};
		set_node expect = node_null;
		set_node * ptbl = (set_node *)oah->tbl.data;
		size_t max = oah->tbl.tbl_size;
		
		arr_mtype_set_to(the_tbl, &node_null);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; 
			check(set_node_is_eq(expect, *arr_mtype_get(ptbl, i)));
		}
		check(oas_intd_is_empty(oah));
		check(0 == oas_intd_entries(oah));
		
		int insert = 1; 
		size_t ins_ind = 1; // insert % max;
		size_t real_ins_ind = 1; //ins_ind;
		check(1 == ins_ind);
		check(1 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(1 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		insert = 17; 
		ins_ind = 1; // insert % max;
		real_ins_ind = ins_ind + 1;
		check(1 == ins_ind);
		check(2 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)((unsigned char *)oas_intd_lookup(oah, &insert))) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_intd_is_empty(oah));
		check(2 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 2; 
		ins_ind = insert % max;
		real_ins_ind = ins_ind + 1; // already taken from above
		check(2 == ins_ind);
		check(3 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_intd_is_empty(oah));
		check(3 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 33; 
		ins_ind = insert % max;
		real_ins_ind = ins_ind + 3; // already taken from above
		check(1 == ins_ind);
		check(4 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_intd_is_empty(oah));
		check(4 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));

		//     1  2   3  4  ...
		// ... 1, 17, 2, 33 ...
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 17);
			check(*oas_intd_iterate(oah, it) == 2);
			check(*oas_intd_iterate(oah, it) == 33);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		key = 17;
		check(*oas_intd_lookup(oah, &key) == 17);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &key)) == 2);

		expect.key = key; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, 2), expect)); // next free slot

		check(!oas_intd_is_empty(oah));
		check(4 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(2)));
		check(!bset_check(bset, bit_vacant(2)));
		
		check(oas_intd_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(2)));
		check(bset_check(bset, bit_vacant(2)));
		
		//     1  2  3  4  ...
		// ... 1, x, 2, 33 ...
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 2);
			check(*oas_intd_iterate(oah, it) == 33);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		check(oas_intd_lookup(oah, &key) == NULL);
		check(!oas_intd_is_empty(oah));
		check(3 == oas_intd_entries(oah));
		
		key = 1000;
		check(oas_intd_lookup(oah, &key) == NULL);
		
		key = 1;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 2;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 33;
		check(*oas_intd_lookup(oah, &key) == key);
		
		check(test_oas_intd_copy(oah));
		
		insert = 18; 
		ins_ind = 2; // insert % max;
		real_ins_ind = ins_ind;
		check(2 == ins_ind);
		check(2 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(4 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		//     1  2   3  4  ...
		// ... 1, 18, 2, 33 ...
		
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 18);
			check(*oas_intd_iterate(oah, it) == 2);
			check(*oas_intd_iterate(oah, it) == 33);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		check(oas_intd_clear(oah) == oah);
		check(oas_intd_is_empty(oah));
		check(0 == oas_intd_entries(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		
		oas_intd_destroy(oah);
	}
	
	// wrap around
	{
		oas_intd oas_, * oah = &oas_;
		bit_set * bset = &oah->tbl.is_taken;
	
		check(oas_intd_create_sz(oah, get_ihash, icmp, 7) == oah);
		
		check(oah->tbl.data);
		check(sizeof(int) == oah->tbl.key_size);
		check(16 == oah->tbl.tbl_size);
		check(bset_num_bits(bset) == 16*2);
		check(bset_num_zeroes(bset) == 16*2);
		check(11 == oah->tbl.elem_left);
		check(get_ihash == oah->tbl.hash);
		check(icmp == oah->tbl.cmp);
		check(oas_intd_is_empty(oah));
	
		arr_mtype_info the_tbl_, * the_tbl = &the_tbl_;
		arr_mtype_info_set(the_tbl, (set_node *)oah->tbl.data, 0, oah->tbl.tbl_size);
		
		int key = 0;
		set_node null_node = {0};
		set_node expect = null_node;
		set_node * ptbl = (set_node *)oah->tbl.data;
		size_t max = oah->tbl.tbl_size;
		arr_mtype_set_to(the_tbl, &null_node);
		
		for (size_t i = 0; i < max; ++i)
		{				
			expect.key = 0; 
			check(set_node_is_eq(expect, *arr_mtype_get(ptbl, i)));
		}
		
		check(oas_intd_is_empty(oah));
		check(0 == oas_intd_entries(oah));
		
		int insert = 14; 
		size_t ins_ind = 14; // insert % max;
		size_t real_ins_ind = 14; //ins_ind;
		check(14 == ins_ind);
		check(14 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(1 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(ins_ind)));
		check(!bset_check(bset, bit_vacant(ins_ind)));
		
		insert = 15; 
		ins_ind = 15; // insert % max;
		real_ins_ind = ins_ind;
		check(15 == ins_ind);
		check(15 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_intd_is_empty(oah));
		check(2 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 16; 
		ins_ind = insert % max;
		real_ins_ind = ins_ind;
		check(0 == ins_ind);
		check(0 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_intd_is_empty(oah));
		check(3 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 31; 
		ins_ind = insert % max;
		real_ins_ind = 1; // already taken from above
		check(15 == ins_ind);
		check(1 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect)); // next free slot

		check(!oas_intd_is_empty(oah));
		check(4 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));

		// 0   1  ... 14  15
		// 16, 31 ... 14, 15
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 16);
			check(*oas_intd_iterate(oah, it) == 31);
			check(*oas_intd_iterate(oah, it) == 14);
			check(*oas_intd_iterate(oah, it) == 15);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		key = 16;
		check(*oas_intd_lookup(oah, &key) == 16);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &key)) == 0);

		expect.key = key; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, 0), expect)); // next free slot

		check(!oas_intd_is_empty(oah));
		check(4 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(0)));
		check(!bset_check(bset, bit_vacant(0)));
		
		check(oas_intd_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(0)));
		check(bset_check(bset, bit_vacant(0)));
		
		// 0  1  ... 14  15
		// x, 31 ... 14, 15
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 31);
			check(*oas_intd_iterate(oah, it) == 14);
			check(*oas_intd_iterate(oah, it) == 15);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		check(oas_intd_lookup(oah, &key) == NULL);
		check(!oas_intd_is_empty(oah));
		check(3 == oas_intd_entries(oah));
		
		key = 14;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 15;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 31;
		check(*oas_intd_lookup(oah, &key) == key);
		
		check(test_oas_intd_copy(oah));
		
		insert = 1; 
		ins_ind = 1; // insert % max;
		real_ins_ind = ins_ind+1;
		check(1 == ins_ind);
		check(2 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(4 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		key = 1000;
		check(oas_intd_lookup(oah, &key) == NULL);
		
		// 0  1   2 ... 14  15
		// x, 31, 1 ... 14, 15
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 31);
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 14);
			check(*oas_intd_iterate(oah, it) == 15);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		insert = 32; 
		ins_ind = insert % max;
		real_ins_ind = ins_ind;
		check(0 == ins_ind);
		check(0 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(5 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));		
		
		key = 31;
		check(oas_intd_remove(oah, &key) == oah);
		check(!bset_check(bset, bit_taken(1)));
		check(bset_check(bset, bit_vacant(1)));
		
		// 0   1  2 ... 14  15
		// 32, x, 1 ... 14, 15
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 32);
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 14);
			check(*oas_intd_iterate(oah, it) == 15);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		check(oas_intd_lookup(oah, &key) == NULL);
		check(!oas_intd_is_empty(oah));
		check(4 == oas_intd_entries(oah));
		
		key = 32;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 1;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 14;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 15;
		check(*oas_intd_lookup(oah, &key) == key);
		
		insert = 2; 
		ins_ind = insert % max;
		real_ins_ind = 3;
		check(2 == ins_ind);
		check(3 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(5 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));		
		
		// 0   1  2  3 ... 14  15
		// 32, x, 1  2 ... 14, 15
		key = 32;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 1;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 14;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 15;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 2;
		check(*oas_intd_lookup(oah, &key) == key);
		
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, 14, 15
		insert = 13; 
		ins_ind = insert % max;
		real_ins_ind = insert;
		check(13 == ins_ind);
		check(13 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(6 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		insert = 29; 
		ins_ind = insert % max;
		real_ins_ind = 1;
		check(13 == ins_ind);
		check(1 == real_ins_ind);
		check(oas_intd_insert(oah, &insert) == oah);
		check(*oas_intd_lookup(oah, &insert) == insert);
		check(arr_mtype_distance(the_tbl,
			(const set_node *)oas_intd_lookup(oah, &insert)) == real_ins_ind);

		expect.key = insert; 
		check(set_node_is_eq(*arr_mtype_get(ptbl, real_ins_ind), expect));

		check(!oas_intd_is_empty(oah));
		check(7 == oas_intd_entries(oah));
		check(bset_check(bset, bit_taken(real_ins_ind)));
		check(!bset_check(bset, bit_vacant(real_ins_ind)));
		
		// 0   1   2  3 ... 13  14  15
		// 32, 29, 1  2 ... 13, 14, 15
		key = 32;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 29;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 1;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 2;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 13;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 14;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 15;
		check(*oas_intd_lookup(oah, &key) == key);
		
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 32);
			check(*oas_intd_iterate(oah, it) == 29);
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 2);
			check(*oas_intd_iterate(oah, it) == 13);
			check(*oas_intd_iterate(oah, it) == 14);
			check(*oas_intd_iterate(oah, it) == 15);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		key = 29;
		check(oas_intd_remove(oah, &key) == oah);
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, 14, 15
		
		key = 14;
		check(oas_intd_remove(oah, &key) == oah);
		check(oas_intd_lookup(oah, &key) == NULL);
		// 0   1  2  3 ... 13  14  15
		// 32, x, 1  2 ... 13, x,  15
		check(oas_intd_entries(oah) == 5);
		
		key = 32;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 1;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 2;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 13;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 15;
		check(*oas_intd_lookup(oah, &key) == key);
		
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 32);
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 2);
			check(*oas_intd_iterate(oah, it) == 13);
			check(*oas_intd_iterate(oah, it) == 15);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		key = 14;
		check(oas_intd_remove(oah, &key) == oah);
		check(oas_intd_lookup(oah, &key) == NULL);
		
		key = 32;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 1;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 2;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 13;
		check(*oas_intd_lookup(oah, &key) == key);
		key = 15;
		check(*oas_intd_lookup(oah, &key) == key);
		
		{
			oas_iterator it_ = OAS_ITER_INIT, * it = &it_;
			check(*oas_intd_iterate(oah, it) == 32);
			check(*oas_intd_iterate(oah, it) == 1);
			check(*oas_intd_iterate(oah, it) == 2);
			check(*oas_intd_iterate(oah, it) == 13);
			check(*oas_intd_iterate(oah, it) == 15);
			check(oas_intd_iterate(oah, it) == NULL);
		}
		
		check(oas_intd_clear(oah) == oah);
		check(oas_intd_is_empty(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		check(0 == oas_intd_entries(oah));
		check(bset_num_zeroes(bset) == bset_num_bits(bset));
		
		oas_intd_destroy(oah);
	}
	
	return true;
}

OA_SET_DEFINE(baz, void *);

static bool test_oas_baz_ptr(void)
{
	oas_baz oas_, * oah = &oas_;
	
	check(oas_baz_create_sz(oah, get_ihash, icmp, 5) == oah);
	
	const void * key = (void *)27;
	check(oas_baz_insert(oah, &key) == oah);
	
	check(oas_baz_insert(oah, &key) == oah);
	check(*oas_baz_lookup(oah, &key) == key);
	
	check(oas_baz_remove(oah, &key) == oah);
	check(oas_baz_lookup(oah, &key) == NULL);
	
	check(oas_baz_insert(oah, &key) == oah);
	check(*oas_baz_lookup(oah, &key) == key);
	
	oas_baz_destroy(oah);
	
	return true;
}

static ftest tests[] = {
	test_oas_void_create_destroy,
	test_oas_void_insert_lookup_remove_clear_iter,
	test_oas_intd_create_destroy,
	test_oas_intd_insert_lookup_remove_clear_iter,
	test_oas_baz_ptr,
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
