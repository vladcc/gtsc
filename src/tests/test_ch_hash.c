#include <stdio.h>
#include <string.h>
#include "test.h"

#define NO_NEXT   NULL
#define NO_NEXT_N ((size_t)~0)
#define NOT_INIT  chh_priv_not_init_val()
#define UNUSED(x) (void)(x)

#include "ch_hash/ch_hash.h"

typedef unsigned char byte;

static inline size_t ihash(const unsigned char * key, size_t len)
{
	UNUSED(len);
	return *(int *)key;
}

static size_t g_hash_called = 0;
static size_t get_ihash(const void * elem)
{
	++g_hash_called;
	return ihash((const unsigned char *)elem, sizeof(int));
}
static bool hash_calls(size_t n)
{
	check(n == g_hash_called);
	return true;
}
static void zero_hash_calls(void)
{
	g_hash_called = 0;
}
static size_t get_hash_calls(void)
{
	return g_hash_called;
}

static size_t g_icmp_called = 0;
static int icmp(const void * k1, const void * k2)
{
	++g_icmp_called;
    int i1 = *((int *)k1);
    int i2 = *((int *)k2);
    return (i1 > i2) - (i1 < i2);
}
static bool icmp_calls(size_t n)
{
	check(n == g_icmp_called);
	return true;
}
static void zero_icmp_calls(void)
{
	g_icmp_called = 0;
}
static size_t get_icmp_calls(void)
{
	return g_icmp_called;
}

static int chhv_get_pool_val(chh_void * chhv, size_t n)
{
	const void * elem = chh_priv_get_elem_addr(&chhv->chain_pool, n);
	return *((int *)elem);
}
static size_t chhv_get_pool_index(chh_void * chhv, const void * elem)
{
	if (!elem)
		goto done;
		
	chh_priv_pool * pool = &chhv->chain_pool;

	void * p = NULL;
	size_t count = 0;
	while ((p = chh_priv_get_elem_addr(pool, count)))
	{
		if (elem == p)
			return count;
		++count;
	}
	
done:
	return NO_NEXT_N;
}
static size_t chhv_get_pool_next(chh_void * chhv, size_t n)
{
	chh_priv_pool * pool = &chhv->chain_pool;
	return chhv_get_pool_index(chhv,
		chh_priv_pool_next_get(pool,
			chh_priv_get_elem_addr(pool, n)));
}
static void * chhv_get_tbl(chh_void * chhv)
{
	return chhv->data;
}
static size_t chhv_pool_head(chh_void * chhv)
{
	return chhv_get_pool_index(chhv,
		chh_priv_pool_get_head(&chhv->chain_pool));
}
static chh_compar chhv_get_cmp(chh_void * chhv)
{
	return chhv->cmp;
}
static chh_get_hash chhv_get_hash(chh_void * chhv)
{
	return chhv->hash;
}
static size_t chhv_kv_pair_size(chh_void * chhv)
{
	return chh_priv_pool_elem_size(&chhv->chain_pool) - sizeof(void*);
}

typedef struct tbl_node {
	int key;
	char pad[sizeof(double) - sizeof(int)];
	double val;
	void * next;
} tbl_node;
static bool chhv_is_tbl_clear(chh_void * chhv)
{
	tbl_node * tbl = chhv_get_tbl(chhv);
	for (size_t i = 0, end = chh_void_cap(chhv); i < end; ++i)
	{
		if (chh_priv_not_init_val() != tbl[i].next)
			return false;
	}
	return true;
}
tbl_node chhv_get_tbl_node_at(chh_void * chhv, size_t ind)
{
	tbl_node * tbl = chhv_get_tbl(chhv);
	return tbl[ind];
}

typedef struct arr_node {
	int key;
	double val;
} arr_node;
#define NODE_ARR_MAX 16
typedef struct node_arr {
	arr_node nodes[NODE_ARR_MAX];
	size_t len;
} node_arr;

static bool node_arr_eq(node_arr * a, node_arr * b)
{
	if (a->len != b->len)
		return false;
		
	for (size_t i = 0; i < a->len; ++i)
	{
		bool found = false;
		for (size_t j = 0; j < b->len; ++j)
		{
			if (a->nodes[i].key == b->nodes[j].key)
			{
				if (a->nodes[i].val == b->nodes[j].val)
				{
					found = true;
					break;
				}
			}
		}
		if (!found)
			return false;
	}
	
	return true;
}

static bool chhv_test_copy(chh_void * chhv)
{
	node_arr arr_tbl;
	memset(&arr_tbl, 0, sizeof(arr_tbl));
	node_arr arr_cpy;
	memset(&arr_cpy, 0, sizeof(arr_cpy));
	
	chh_void cpy_, * cpy = &cpy_;
	bool is_ok = false;
	*cpy = chh_void_copy(chhv, &is_ok);
	check(is_ok);
	
	check(chh_void_elems(chhv) == chh_void_elems(cpy));
	check(chh_priv_pool_curr_cap(&chhv->chain_pool)
		== chh_priv_pool_curr_cap(&cpy->chain_pool));
	
	chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
	chh_iterator cpy_iter_, * cpy_iter = &cpy_iter_;
	
	chh_iterator_init(chhv_iter);
	chh_iterator_init(cpy_iter);
	const void * cvh_key = NULL;
	void * cvh_val = NULL;
	const void * cpy_key = NULL;
	void * cpy_val = NULL;
	
	size_t count = 0;
	while ((cvh_key = chh_void_iterate(chhv, chhv_iter, &cvh_val)))
	{
		cpy_key = chh_void_iterate(cpy, cpy_iter, &cpy_val);
		
		arr_tbl.nodes[count].key = *(int *)cvh_key;
		arr_tbl.nodes[count].val = *(double *)cvh_val;
		
		arr_cpy.nodes[count].key = *(int *)cpy_key;
		arr_cpy.nodes[count].val = *(double *)cpy_val;
		
		++count;
	}
	
	arr_tbl.len = arr_cpy.len = count;
	check(chh_void_elems(chhv) == count);
	
	check(node_arr_eq(&arr_tbl, &arr_cpy));
	
	cpy_key = chh_void_iterate(cpy, cpy_iter, &cpy_val);
	check(NULL == cvh_key);
	check(NULL == cpy_key);
	check(cvh_key == cpy_key);
	
	check(NULL == cvh_val);
	check(NULL == cpy_val);
	check(cvh_val == cpy_val);
	
	chh_void_destroy(cpy);
	return true;
}

static bool test_chh_void_create(void)
{
	zero_icmp_calls();
	zero_hash_calls();
	
	{
		chh_void cvh_, * cvh = &cvh_;
		check(chh_void_create_sz_pl(cvh,
			sizeof(int), sizeof(double), get_ihash, icmp, 3, 5) == cvh);
		
		check(0 == cvh->elems);
		check(chh_void_elems(cvh) == cvh->elems);
		check(chh_void_is_empty(cvh));
		check(chh_void_cap(cvh) == 4);
		check(sizeof(int) == cvh->key_size);
		check(sizeof(double) == cvh->val_size);
		check(sizeof(int) + (sizeof(double) - sizeof(int)) == cvh->val_offs);
		check(5 == chh_priv_pool_curr_cap(&cvh->chain_pool));
		
		check(chhv_kv_pair_size(cvh) == cvh->val_offs + cvh->val_size);
		
		check(chhv_is_tbl_clear(cvh));
		check(chhv_get_tbl(cvh));
		check(chhv_get_cmp(cvh) == icmp);
		check(chhv_get_hash(cvh) == get_ihash);
		
		check(chhv_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		chh_void_destroy(cvh);
	}
	
	{
		chh_void cvh_, * cvh = &cvh_;
		check(chh_void_create(cvh,
			sizeof(int), sizeof(double), get_ihash, icmp) == cvh);
		
		check(CH_HASH_DEFAULT_POOL == chh_priv_pool_curr_cap(&cvh->chain_pool));
		check(0 == cvh->elems);
		check(chh_void_elems(cvh) == cvh->elems);
		check(chh_void_is_empty(cvh));
		check(chh_void_cap(cvh) == CH_HASH_DEFAULT_SIZE);
		check(sizeof(int) == cvh->key_size);
		check(sizeof(double) == cvh->val_size);
		check(sizeof(int) + (sizeof(double) - sizeof(int)) == cvh->val_offs);
		check(chhv_kv_pair_size(cvh) == cvh->val_offs + cvh->val_size);
			
		check(chhv_is_tbl_clear(cvh));
		check(chhv_get_tbl(cvh));
		check(chhv_get_cmp(cvh) == icmp);
		check(chhv_get_hash(cvh) == get_ihash);
		
		check(chhv_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		chh_void_destroy(cvh);
	}
	
	return true;
}

#define ins_val_make(x) ((double)((x)*10+2.0)) // some specific bit pattern

static bool test_chh_void_insert(void)
{
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);
	
	int key = 1;
	double val = ins_val_make(key);
	
	check(chh_void_insert(cvh, &key, &val) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(chh_void_elems(cvh) == 1);
	check(!chh_void_is_empty(cvh));
	
	check(icmp_calls(0));
	check(hash_calls(1));
	
	tbl_node node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(NO_NEXT == node.next);
	
	key = 5;
	val = ins_val_make(key);
	check(chh_void_insert(cvh, &key, &val) == cvh);
	// 0 = x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(chh_void_elems(cvh) == 2);
	
	check(icmp_calls(1));
	check(hash_calls(2));
	
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(0 == chhv_get_pool_index(cvh, node.next));
	
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	key = 4;
	val = ins_val_make(key);
	check(chh_void_insert(cvh, &key, &val) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(chh_void_elems(cvh) == 3);
	
	check(icmp_calls(1));
	check(hash_calls(3));
	
	node = chhv_get_tbl_node_at(cvh, 0);
	check(4 == node.key);
	check(ins_val_make(4) == node.val);
	check(NO_NEXT == node.next);
	
	key = 7;
	val = ins_val_make(key);
	check(chh_void_insert(cvh, &key, &val) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = 7 -> x
	check(chh_void_elems(cvh) == 4);
	
	check(icmp_calls(1));
	check(hash_calls(4));
	
	node = chhv_get_tbl_node_at(cvh, 3);
	check(7 == node.key);
	check(ins_val_make(7) == node.val);
	check(NO_NEXT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	
	// growth event
	key = 12;
	val = ins_val_make(key);
	check(chh_void_cap(cvh) == 4);
	check(chh_void_insert(cvh, &key, &val) == cvh);
	check(chh_void_cap(cvh) == 8);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(chh_void_elems(cvh) == 5);
	
	check(icmp_calls(2));
	check(hash_calls(9));
	
	key = 20;
	val = ins_val_make(key);
	check(chh_void_insert(cvh, &key, &val) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 20 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(chh_void_elems(cvh) == 6);
	
	check(icmp_calls(4));
	check(hash_calls(10));
	
	node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(NO_NEXT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 4);
	check(4 == node.key);
	check(ins_val_make(4) == node.val);
	check(1 == chhv_get_pool_index(cvh, node.next));
	
	check(chhv_get_pool_val(cvh, 1) == 20);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 12);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	node = chhv_get_tbl_node_at(cvh, 5);
	check(5 == node.key);
	check(ins_val_make(5) == node.val);
	check(NO_NEXT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 6);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 7);
	check(7 == node.key);
	check(ins_val_make(7) == node.val);
	check(NO_NEXT == node.next);
	
	chh_void_destroy(cvh);
	
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	return true;
}

static chh_void * chhv_ins(chh_void * cvh, int * arr, size_t len)
{
	double val = 0;
	for (size_t i = 0; i < len; ++i)
	{
		val = ins_val_make(arr[i]);
		chh_void_insert(cvh, arr+i, &val);
	}
	return cvh;
}

static bool chhv_get4(chh_void * cvh)
{
	int arr[] = {1, 5, 4, 7};
	chhv_ins(cvh, arr, 4);
	check(4 == chh_void_elems(cvh));
	check(!chh_void_is_empty(cvh));
	return true;
}

static bool chhv_get6(chh_void * cvh)
{
	int arr[] = {12, 20};
	chhv_get4(cvh);
	chhv_ins(cvh, arr, 2);
	check(6 == chh_void_elems(cvh));
	check(!chh_void_is_empty(cvh));
	return true;
}

static bool test_chh_void_iterate(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);
		
	chhv_get4(cvh);
	
	chh_iterator iter_, * iter = &iter_;
	chh_iterator_init(iter);
	
	const void * key = NULL;
	void * val = NULL;
	
	key = chh_void_iterate(cvh, iter, &val);
	check(4 == *(int *)key && ins_val_make(4) == *(double *)val);
	key = chh_void_iterate(cvh, iter, &val);
	check(1 == *(int *)key && ins_val_make(1) == *(double *)val);
	key = chh_void_iterate(cvh, iter, &val);
	check(5 == *(int *)key && ins_val_make(5) == *(double *)val);
	key = chh_void_iterate(cvh, iter, &val);
	check(7 == *(int *)key && ins_val_make(7) == *(double *)val);
	
	key = chh_void_iterate(cvh, iter, &val);
	check(NULL == key);
	check(NULL == val);
	
	chhv_get6(cvh);
	
	chh_iterator_init(iter);
	key = chh_void_iterate(cvh, iter, &val);
	check(1 == *(int *)key && ins_val_make(1) == *(double *)val);
	key = chh_void_iterate(cvh, iter, &val);
	check(4 == *(int *)key && ins_val_make(4) == *(double *)val);
	key = chh_void_iterate(cvh, iter, &val);
	check(20 == *(int *)key && ins_val_make(20) == *(double *)val);
	key = chh_void_iterate(cvh, iter, &val);
	check(12 == *(int *)key && ins_val_make(12) == *(double *)val);
	key = chh_void_iterate(cvh, iter, &val);
	check(5 == *(int *)key && ins_val_make(5) == *(double *)val);
	key = chh_void_iterate(cvh, iter, &val);
	check(7 == *(int *)key && ins_val_make(7) == *(double *)val);
	
	key = chh_void_iterate(cvh, iter, &val);
	check(NULL == key);
	check(NULL == val);
	
	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_rewrite(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);
		
	chhv_get4(cvh);
	
	int key = 1;
	double val = ins_val_make(key*10);
	check(chh_void_insert(cvh, &key, &val) == cvh);
	
	key = 4;
	val = ins_val_make(key*10);
	check(chh_void_insert(cvh, &key, &val) == cvh);
	
	key = 5;
	val = ins_val_make(key*10);
	check(chh_void_insert(cvh, &key, &val) == cvh);
		
	chh_iterator iter_, * iter = &iter_;
	chh_iterator_init(iter);
	
	const void * pkey = NULL;
	void * pval = NULL;
	
	pkey = chh_void_iterate(cvh, iter, &pval);
	check(4 == *(int *)pkey && ins_val_make(40) == *(double *)pval);
	pkey = chh_void_iterate(cvh, iter, &pval);
	check(1 == *(int *)pkey && ins_val_make(10) == *(double *)pval);
	pkey = chh_void_iterate(cvh, iter, &pval);
	check(5 == *(int *)pkey && ins_val_make(50) == *(double *)pval);
	pkey = chh_void_iterate(cvh, iter, &pval);
	check(7 == *(int *)pkey && ins_val_make(7) == *(double *)pval);
	
	pkey = chh_void_iterate(cvh, iter, &pval);
	check(NULL == pkey);
	check(NULL == pval);
		
	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_lookup(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);
	
	int key = 4;
	const void * pval = NULL;
	check(NULL == chh_void_lookup(cvh, &key));
	
	key = 0;
	check(NULL == chh_void_lookup(cvh, &key));
	
	chhv_get4(cvh);
	key = 4;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1000;
	check(NULL == chh_void_lookup(cvh, &key));
	
	key = 5;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = -1000;
	check(NULL == chh_void_lookup(cvh, &key));
	
	chhv_get6(cvh);
	
	key = 5;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 20;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_clear(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);
		
	chhv_get4(cvh);
	
	int key = 4;
	const void * pval = NULL;
	
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 5;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	size_t h_calls = get_hash_calls();
	size_t cmp_calls = get_icmp_calls();
	check(chh_void_clear(cvh) == cvh);
	check(0 == chh_void_elems(cvh));
	check(chh_void_is_empty(cvh));
	hash_calls(h_calls);
	icmp_calls(cmp_calls);
	
	key = 4;
	pval = chh_void_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 1;
	pval = chh_void_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 5;
	pval = chh_void_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 7;
	pval = chh_void_lookup(cvh, &key);
	check(NULL == pval);
	
	chhv_get6(cvh);
	
	key = 1;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 5;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 4;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 20;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 12;
	pval = chh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_copy(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);
	
	check(chhv_test_copy(cvh));
	
	chhv_get4(cvh);
	check(chhv_test_copy(cvh));
	
	chhv_get6(cvh);
	check(chhv_test_copy(cvh));
	
	chh_void_destroy(cvh);
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 5) == cvh);
	
	chhv_get6(cvh);
	check(chhv_test_copy(cvh));
	
	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_remove_from_front(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);

	int arr[] = {1, 5, 9};
	chhv_ins(cvh, arr, 3);
	check(3 == chh_void_elems(cvh));
	check(!chh_void_is_empty(cvh));
	check(4 == chh_void_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(1 == chhv_get_pool_index(cvh, node.next));
	
	check(chhv_get_pool_val(cvh, 1) == 9);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(chh_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(chh_void_lookup(cvh, &key));
	key = 5;
	check(chh_void_lookup(cvh, &key));
	key = 9;
	check(chh_void_lookup(cvh, &key));
	check(3 == chh_void_elems(cvh));
	
	key = 1;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(chh_void_lookup(cvh, &key));
	key = 5;
	check(chh_void_lookup(cvh, &key));
	check(2 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh,0) == NO_NEXT_N);
		
		check(1 == chhv_pool_head(cvh));
	}
	
	key = 9;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(chh_void_lookup(cvh, &key));
	check(1 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(5 == node.key);
		check(ins_val_make(5) == node.val);
		check(NO_NEXT == node.next);
		
		check(0 == chhv_pool_head(cvh));
	}

	key = 5;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chh_void_elems(cvh));
	check(chh_void_is_empty(cvh));
	
	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(0 == chhv_pool_head(cvh));
	}

	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_remove_from_tail(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);

	int arr[] = {1, 5, 9};
	chhv_ins(cvh, arr, 3);
	check(3 == chh_void_elems(cvh));
	check(!chh_void_is_empty(cvh));
	check(4 == chh_void_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(1 == chhv_get_pool_index(cvh, node.next));

	check(chhv_get_pool_val(cvh, 1) == 9);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(chh_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(chh_void_lookup(cvh, &key));
	key = 5;
	check(chh_void_lookup(cvh, &key));
	key = 9;
	check(chh_void_lookup(cvh, &key));
	check(3 == chh_void_elems(cvh));
	
	key = 5;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 9 -> x
	
	key = 1;
	check(chh_void_lookup(cvh, &key));
	key = 9;
	check(chh_void_lookup(cvh, &key));
	check(2 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(1 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 1) == 9);
		check(chhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == chhv_pool_head(cvh));
	}
	
	key = 9;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> x
	
	key = 1;
	check(chh_void_lookup(cvh, &key));
	check(1 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(NO_NEXT == node.next);
		
		check(1 == chhv_pool_head(cvh));
	}

	key = 1;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chh_void_elems(cvh));
	check(chh_void_is_empty(cvh));
	
	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(1 == chhv_pool_head(cvh));
	}

	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_remove_from_middle(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);

	int arr[] = {1, 5, 9};
	chhv_ins(cvh, arr, 3);
	check(3 == chh_void_elems(cvh));
	check(!chh_void_is_empty(cvh));
	check(4 == chh_void_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(1 == chhv_get_pool_index(cvh, node.next));

	check(chhv_get_pool_val(cvh, 1) == 9);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key = 10;
	check(chh_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(chh_void_lookup(cvh, &key));
	key = 9;
	check(chh_void_lookup(cvh, &key));
	key = 5;
	check(chh_void_lookup(cvh, &key));
	check(3 == chh_void_elems(cvh));
	
	key = 9;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(chh_void_lookup(cvh, &key));
	key = 5;
	check(chh_void_lookup(cvh, &key));
	check(2 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == chhv_pool_head(cvh));
	}
	
	key = 1;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(chh_void_lookup(cvh, &key));
	check(1 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(5 == node.key);
		check(ins_val_make(5) == node.val);
		check(NO_NEXT == node.next);
		
		check(0 == chhv_pool_head(cvh));
	}

	key = 5;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chh_void_elems(cvh));
	check(chh_void_is_empty(cvh));
	
	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(0 == chhv_pool_head(cvh));
	}

	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_remove_reinsert_list(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);

	int arr[] = {1, 5, 9};
	chhv_ins(cvh, arr, 3);
	check(3 == chh_void_elems(cvh));
	check(!chh_void_is_empty(cvh));
	check(4 == chh_void_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(1 == chhv_get_pool_index(cvh, node.next));

	check(chhv_get_pool_val(cvh, 1) == 9);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(chh_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(chh_void_lookup(cvh, &key));
	key = 5;
	check(chh_void_lookup(cvh, &key));
	key = 9;
	check(chh_void_lookup(cvh, &key));
	check(3 == chh_void_elems(cvh));
	
	key = 9;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(chh_void_lookup(cvh, &key));
	key = 5;
	check(chh_void_lookup(cvh, &key));
	check(2 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == chhv_pool_head(cvh));
	}
	
	key = 9;
	double val = ins_val_make(key);
	chh_void_insert(cvh, &key, &val);
	check(3 == chh_void_elems(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	
	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(1 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 1) == 9);
		check(chhv_get_pool_next(cvh, 1) == 0);
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == chhv_pool_head(cvh));
	}
	
	key = 1;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(chh_void_lookup(cvh, &key));
	key = 5;
	check(chh_void_lookup(cvh, &key));
	check(2 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == chhv_pool_head(cvh));
	}
	
	key = 1;
	val = ins_val_make(key);
	chh_void_insert(cvh, &key, &val);
	check(3 == chh_void_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 1 -> 5 -> x
	
	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(1 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 1) == 1);
		check(chhv_get_pool_next(cvh, 1) == 0);
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == chhv_pool_head(cvh));
	}
	
	// repeat; remove from tail
	
	key = 5;
	check(chh_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!chh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 1 -> x
	
	key = 9;
	check(chh_void_lookup(cvh, &key));
	key = 1;
	check(chh_void_lookup(cvh, &key));
	check(2 == chh_void_elems(cvh));

	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(1 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 1) == 1);
		check(chhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == chhv_pool_head(cvh));
	}
	
	key = 5;
	val = ins_val_make(key);
	chh_void_insert(cvh, &key, &val);
	check(3 == chh_void_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 5 -> 1 -> x
	
	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = chhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = chhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = chhv_get_tbl_node_at(cvh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == 1);
		check(chhv_get_pool_val(cvh, 1) == 1);
		check(chhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(2 == chhv_pool_head(cvh));
	}
	
	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_remove_reinsert_trivial(void)
{
	chh_void cvh_, * cvh = &cvh_;
	
	check(chh_void_create_sz(cvh,
		sizeof(int), sizeof(double), get_ihash, icmp, 4) == cvh);
		
	tbl_node node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	int key = 1;
	double val = ins_val_make(key);
	
	check(chh_void_insert(cvh, &key, &val) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(chh_void_elems(cvh) == 1);
	check(!chh_void_is_empty(cvh));
	
	
	{
		chh_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chh_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = chh_void_iterate(cvh, chhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
	}
	
	node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(NO_NEXT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	check(chh_void_remove(cvh, &key) == cvh);
	check(chh_void_is_empty(cvh));
	
	node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	chh_void_destroy(cvh);
	return true;
}

static bool test_chh_void_remove(void)
{
	check(test_chh_void_remove_from_front());
	check(test_chh_void_remove_from_tail());
	check(test_chh_void_remove_from_middle());
	check(test_chh_void_remove_reinsert_trivial());
	check(test_chh_void_remove_reinsert_list());
	return true;
}

CH_HASH_DEFINE(int_dbl, int, double);

static int chhid_get_pool_val(chh_int_dbl * chhid, size_t n)
{
	return chhv_get_pool_val(&chhid->tbl, n);
}
static size_t chhid_get_pool_next(chh_int_dbl * chhid, size_t n)
{
	return chhv_get_pool_next(&chhid->tbl, n);
}
static size_t chhid_get_pool_index(chh_int_dbl * chhid, const void * elem)
{
	return chhv_get_pool_index(&chhid->tbl, elem);
}
static void * chhid_get_tbl(chh_int_dbl * chhid)
{
	return chhv_get_tbl(&chhid->tbl);
}
static size_t chhid_pool_head(chh_int_dbl * chhid)
{
	return chhv_pool_head(&chhid->tbl);
}
static chh_compar chhid_get_cmp(chh_int_dbl * chhid)
{
	return chhv_get_cmp(&chhid->tbl);
}
static chh_get_hash chhid_get_hash(chh_int_dbl * chhid)
{
	return chhv_get_hash(&chhid->tbl);
}

static bool chhid_is_tbl_clear(chh_int_dbl * chhid)
{
	return chhv_is_tbl_clear(&chhid->tbl);
}
tbl_node chhid_get_tbl_node_at(chh_int_dbl * chhid, size_t ind)
{
	return chhv_get_tbl_node_at(&chhid->tbl, ind);
}
static bool chhid_test_copy(chh_int_dbl * chhid)
{
	check(chhv_test_copy(&chhid->tbl));
	return true;
}
static size_t chhid_kv_pair_size(chh_int_dbl * chhid)
{
	return chhv_kv_pair_size(&chhid->tbl);
}



static bool test_chh_int_dbl_create(void)
{
	zero_icmp_calls();
	zero_hash_calls();
	
	{
		chh_int_dbl cidh_, * cidh = &cidh_;
		check(chh_int_dbl_create_sz_pl(cidh, get_ihash, icmp, 3, 5) == cidh);
		
		check(0 == cidh->tbl.elems);
		check(chh_int_dbl_elems(cidh) == cidh->tbl.elems);
		check(chh_int_dbl_is_empty(cidh));
		check(chh_int_dbl_cap(cidh) == 4);
		check(sizeof(int) == cidh->tbl.key_size);
		check(sizeof(double) == cidh->tbl.val_size);
		check(sizeof(int) + (sizeof(double) - sizeof(int)) == cidh->tbl.val_offs);
		check(5 == chh_priv_pool_curr_cap(&cidh->tbl.chain_pool));
		
		check(chhid_kv_pair_size(cidh) == cidh->tbl.val_offs + cidh->tbl.val_size);
		
		check(chhid_is_tbl_clear(cidh));
		check(chhid_get_tbl(cidh));
		check(chhid_get_cmp(cidh) == icmp);
		check(chhid_get_hash(cidh) == get_ihash);
		
		check(chhid_test_copy(cidh));
		
		{
			// use named copy function
			bool is_ok = false;
			chh_int_dbl copy_, * copy = &copy_;
			
			*copy = chh_int_dbl_copy(cidh, &is_ok);
			check(is_ok);
			
			check(0 == copy->tbl.elems);
			check(chh_int_dbl_elems(copy) == copy->tbl.elems);
			check(chh_int_dbl_is_empty(copy));
			check(chh_int_dbl_cap(copy) == 1);
			check(sizeof(int) == copy->tbl.key_size);
			check(sizeof(double) == copy->tbl.val_size);
			check(sizeof(int) + (sizeof(double) - sizeof(int)) == copy->tbl.val_offs);
			check(5 == chh_priv_pool_curr_cap(&copy->tbl.chain_pool));
			
			chh_int_dbl_destroy(copy);
		}
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		chh_int_dbl_destroy(cidh);
	}
	
	{
		chh_int_dbl cidh_, * cidh = &cidh_;
		check(chh_int_dbl_create(cidh, get_ihash, icmp) == cidh);
		
		check(0 == cidh->tbl.elems);
		check(chh_int_dbl_elems(cidh) == cidh->tbl.elems);
		check(chh_int_dbl_is_empty(cidh));
		check(chh_int_dbl_cap(cidh) == CH_HASH_DEFAULT_SIZE);
		check(sizeof(int) == cidh->tbl.key_size);
		check(sizeof(double) == cidh->tbl.val_size);
		check(sizeof(int) + (sizeof(double) - sizeof(int)) == cidh->tbl.val_offs);
		check(chhid_kv_pair_size(cidh) == cidh->tbl.val_offs + cidh->tbl.val_size);
		check(CH_HASH_DEFAULT_POOL
			== chh_priv_pool_curr_cap(&cidh->tbl.chain_pool));
			
		check(chhid_is_tbl_clear(cidh));
		check(chhid_get_tbl(cidh));
		check(chhid_get_cmp(cidh) == icmp);
		check(chhid_get_hash(cidh) == get_ihash);
		
		check(chhid_test_copy(cidh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		chh_int_dbl_destroy(cidh);
	}
	
	return true;
}

#define ins_val_make(x) ((double)((x)*10+2.0)) // some specific bit pattern

static bool test_chh_int_dbl_insert(void)
{
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);
	
	int key = 1;
	double val = ins_val_make(key);
	
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(chh_int_dbl_elems(cidh) == 1);
	check(!chh_int_dbl_is_empty(cidh));
	
	check(icmp_calls(0));
	check(hash_calls(1));
	
	tbl_node node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(NO_NEXT == node.next);
	
	key = 5;
	val = ins_val_make(key);
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	// 0 = x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(chh_int_dbl_elems(cidh) == 2);
	
	check(icmp_calls(1));
	check(hash_calls(2));
	
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(0 == chhid_get_pool_index(cidh, node.next));
	
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
	
	key = 4;
	val = ins_val_make(key);
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(chh_int_dbl_elems(cidh) == 3);
	
	check(icmp_calls(1));
	check(hash_calls(3));
	
	node = chhid_get_tbl_node_at(cidh, 0);
	check(4 == node.key);
	check(ins_val_make(4) == node.val);
	check(NO_NEXT == node.next);
	
	key = 7;
	val = ins_val_make(key);
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = 7 -> x
	check(chh_int_dbl_elems(cidh) == 4);
	
	check(icmp_calls(1));
	check(hash_calls(4));
	
	node = chhid_get_tbl_node_at(cidh, 3);
	check(7 == node.key);
	check(ins_val_make(7) == node.val);
	check(NO_NEXT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	
	// growth event
	key = 12;
	val = ins_val_make(key);
	check(chh_int_dbl_cap(cidh) == 4);
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	check(chh_int_dbl_cap(cidh) == 8);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(chh_int_dbl_elems(cidh) == 5);
	
	check(icmp_calls(2));
	check(hash_calls(9));
	
	key = 20;
	val = ins_val_make(key);
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 20 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(chh_int_dbl_elems(cidh) == 6);
	
	check(icmp_calls(4));
	check(hash_calls(10));
	
	node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(NO_NEXT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 4);
	check(4 == node.key);
	check(ins_val_make(4) == node.val);
	check(1 == chhid_get_pool_index(cidh, node.next));
	
	check(chhid_get_pool_val(cidh, 1) == 20);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 12);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
	
	node = chhid_get_tbl_node_at(cidh, 5);
	check(5 == node.key);
	check(ins_val_make(5) == node.val);
	check(NO_NEXT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 6);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 7);
	check(7 == node.key);
	check(ins_val_make(7) == node.val);
	check(NO_NEXT == node.next);
	
	chh_int_dbl_destroy(cidh);
	
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	return true;
}

static chh_int_dbl * chhid_ins(chh_int_dbl * cidh, int * arr, size_t len)
{
	double val = 0;
	for (size_t i = 0; i < len; ++i)
	{
		val = ins_val_make(arr[i]);
		chh_int_dbl_insert(cidh, arr+i, &val);
	}
	return cidh;
}

static bool chhid_get4(chh_int_dbl * cidh)
{
	int arr[] = {1, 5, 4, 7};
	chhid_ins(cidh, arr, 4);
	check(4 == chh_int_dbl_elems(cidh));
	check(!chh_int_dbl_is_empty(cidh));
	return true;
}

static bool chhid_get6(chh_int_dbl * cidh)
{
	int arr[] = {12, 20};
	chhid_get4(cidh);
	chhid_ins(cidh, arr, 2);
	check(6 == chh_int_dbl_elems(cidh));
	check(!chh_int_dbl_is_empty(cidh));
	return true;
}

static bool test_chh_int_dbl_iterate(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);
		
	chhid_get4(cidh);
	
	chh_iterator iter_, * iter = &iter_;
	chh_iterator_init(iter);
	
	const int * key = NULL;
	double * val = NULL;
	
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(4 == *key && ins_val_make(4) == *val);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(1 == *key && ins_val_make(1) == *val);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(5 == *key && ins_val_make(5) == *val);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(7 == *key && ins_val_make(7) == *val);
	
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(NULL == key);
	check(NULL == val);
	
	chhid_get6(cidh);
	
	chh_iterator_init(iter);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(1 == *key && ins_val_make(1) == *val);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(4 == *key && ins_val_make(4) == *val);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(20 == *key && ins_val_make(20) == *val);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(12 == *key && ins_val_make(12) == *val);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(5 == *key && ins_val_make(5) == *val);
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(7 == *key && ins_val_make(7) == *val);
	
	key = chh_int_dbl_iterate(cidh, iter, &val);
	check(NULL == key);
	check(NULL == val);
	
	chh_int_dbl_destroy(cidh);
	return true;
}

static bool test_chh_int_dbl_rewrite(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);
		
	chhid_get4(cidh);
	
	int key = 1;
	double val = ins_val_make(key*10);
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	
	key = 4;
	val = ins_val_make(key*10);
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	
	key = 5;
	val = ins_val_make(key*10);
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
		
	chh_iterator iter_, * iter = &iter_;
	chh_iterator_init(iter);
	
	const int * pkey = NULL;
	double * pval = NULL;
	
	pkey = chh_int_dbl_iterate(cidh, iter, &pval);
	check(4 == *pkey && ins_val_make(40) == *pval);
	pkey = chh_int_dbl_iterate(cidh, iter, &pval);
	check(1 == *pkey && ins_val_make(10) == *pval);
	pkey = chh_int_dbl_iterate(cidh, iter, &pval);
	check(5 == *pkey && ins_val_make(50) == *pval);
	pkey = chh_int_dbl_iterate(cidh, iter, &pval);
	check(7 == *pkey && ins_val_make(7) == *pval);
	
	pkey = chh_int_dbl_iterate(cidh, iter, &pval);
	check(NULL == pkey);
	check(NULL == pval);
		
	chh_int_dbl_destroy(cidh);
	return true;
}


static bool test_chh_int_dbl_lookup(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);
	
	int key = 4;
	const double * pval = NULL;
	check(NULL == chh_int_dbl_lookup(cidh, &key));
	
	key = 0;
	check(NULL == chh_int_dbl_lookup(cidh, &key));
	
	chhid_get4(cidh);
	key = 4;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 1;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 1000;
	check(NULL == chh_int_dbl_lookup(cidh, &key));
	
	key = 5;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 7;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = -1000;
	check(NULL == chh_int_dbl_lookup(cidh, &key));
	
	chhid_get6(cidh);
	
	key = 5;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 20;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	chh_int_dbl_destroy(cidh);
	return true;
}

static bool test_chh_int_dbl_clear(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);
		
	chhid_get4(cidh);
	
	int key = 4;
	const double * pval = NULL;
	
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 1;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 5;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 7;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	size_t h_calls = get_hash_calls();
	size_t cmp_calls = get_icmp_calls();
	check(chh_int_dbl_clear(cidh) == cidh);
	check(0 == chh_int_dbl_elems(cidh));
	check(chh_int_dbl_is_empty(cidh));
	hash_calls(h_calls);
	icmp_calls(cmp_calls);
	
	key = 4;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(NULL == pval);
	
	key = 1;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(NULL == pval);
	
	key = 5;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(NULL == pval);
	
	key = 7;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(NULL == pval);
	
	chhid_get6(cidh);
	
	key = 1;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 5;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 7;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 4;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 20;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	key = 12;
	pval = chh_int_dbl_lookup(cidh, &key);
	check(ins_val_make(key) == *pval);
	
	chh_int_dbl_destroy(cidh);
	return true;
}

static bool test_chh_int_dbl_copy(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);
	
	check(chhid_test_copy(cidh));
	
	chhid_get4(cidh);
	check(chhid_test_copy(cidh));
	
	chhid_get6(cidh);
	check(chhid_test_copy(cidh));
	
	chh_int_dbl_destroy(cidh);
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 5) == cidh);
	
	chhid_get6(cidh);
	check(chhid_test_copy(cidh));
	
	chh_int_dbl_destroy(cidh);
	return true;
}

static bool test_chh_int_dbl_remove_from_front(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);

	int arr[] = {1, 5, 9};
	chhid_ins(cidh, arr, 3);
	check(3 == chh_int_dbl_elems(cidh));
	check(!chh_int_dbl_is_empty(cidh));
	check(4 == chh_int_dbl_cap(cidh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(1 == chhid_get_pool_index(cidh, node.next));
	
	check(chhid_get_pool_val(cidh, 1) == 9);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);

	int key = 10;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 9;
	check(chh_int_dbl_lookup(cidh, &key));
	check(3 == chh_int_dbl_elems(cidh));
	
	key = 1;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 1;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	check(2 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 9);
		check((double)(*cidh_val) == ins_val_make(9));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh,0) == NO_NEXT_N);
		
		check(1 == chhid_pool_head(cidh));
	}
	
	key = 9;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 9;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	check(1 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(5 == node.key);
		check(ins_val_make(5) == node.val);
		check(NO_NEXT == node.next);
		
		check(0 == chhid_pool_head(cidh));
	}

	key = 5;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 5;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chh_int_dbl_elems(cidh));
	check(chh_int_dbl_is_empty(cidh));
	
	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 1);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		check(0 == chhid_pool_head(cidh));
	}

	chh_int_dbl_destroy(cidh);
	return true;
}

static bool test_chh_int_dbl_remove_from_tail(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);

	int arr[] = {1, 5, 9};
	chhid_ins(cidh, arr, 3);
	check(3 == chh_int_dbl_elems(cidh));
	check(!chh_int_dbl_is_empty(cidh));
	check(4 == chh_int_dbl_cap(cidh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(1 == chhid_get_pool_index(cidh, node.next));

	check(chhid_get_pool_val(cidh, 1) == 9);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);

	int key = 10;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 9;
	check(chh_int_dbl_lookup(cidh, &key));
	check(3 == chh_int_dbl_elems(cidh));
	
	key = 5;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 5;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 1 -> 9 -> x
	
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 9;
	check(chh_int_dbl_lookup(cidh, &key));
	check(2 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 9);
		check((double)(*cidh_val) == ins_val_make(9));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(1 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 1) == 9);
		check(chhid_get_pool_next(cidh, 1) == NO_NEXT_N);
		
		check(0 == chhid_pool_head(cidh));
	}
	
	key = 9;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 9;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 1 -> x
	
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	check(1 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(NO_NEXT == node.next);
		
		check(1 == chhid_pool_head(cidh));
	}

	key = 1;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 1;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chh_int_dbl_elems(cidh));
	check(chh_int_dbl_is_empty(cidh));
	
	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 1);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		check(1 == chhid_pool_head(cidh));
	}

	chh_int_dbl_destroy(cidh);
	return true;
}

static bool test_chh_int_dbl_remove_from_middle(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);

	int arr[] = {1, 5, 9};
	chhid_ins(cidh, arr, 3);
	check(3 == chh_int_dbl_elems(cidh));
	check(!chh_int_dbl_is_empty(cidh));
	check(4 == chh_int_dbl_cap(cidh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(1 == chhid_get_pool_index(cidh, node.next));

	check(chhid_get_pool_val(cidh, 1) == 9);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
	
	int key = 10;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 9;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	check(3 == chh_int_dbl_elems(cidh));
	
	key = 9;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 9;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	check(2 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(1 == chhid_pool_head(cidh));
	}
	
	key = 1;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 1;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	check(1 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(5 == node.key);
		check(ins_val_make(5) == node.val);
		check(NO_NEXT == node.next);
		
		check(0 == chhid_pool_head(cidh));
	}

	key = 5;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 5;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chh_int_dbl_elems(cidh));
	check(chh_int_dbl_is_empty(cidh));
	
	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 1);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		check(0 == chhid_pool_head(cidh));
	}

	chh_int_dbl_destroy(cidh);
	return true;
}


static bool test_chh_int_dbl_remove_reinsert_list(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);

	int arr[] = {1, 5, 9};
	chhid_ins(cidh, arr, 3);
	check(3 == chh_int_dbl_elems(cidh));
	check(!chh_int_dbl_is_empty(cidh));
	check(4 == chh_int_dbl_cap(cidh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(1 == chhid_get_pool_index(cidh, node.next));

	check(chhid_get_pool_val(cidh, 1) == 9);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);

	int key = 10;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 9;
	check(chh_int_dbl_lookup(cidh, &key));
	check(3 == chh_int_dbl_elems(cidh));
	
	key = 9;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 9;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	check(2 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(1 == chhid_pool_head(cidh));
	}
	
	key = 9;
	double val = ins_val_make(key);
	chh_int_dbl_insert(cidh, &key, &val);
	check(3 == chh_int_dbl_elems(cidh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	
	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 9);
		check((double)(*cidh_val) == ins_val_make(9));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(1 == node.key);
		check(ins_val_make(1) == node.val);
		check(1 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 1) == 9);
		check(chhid_get_pool_next(cidh, 1) == 0);
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(2 == chhid_pool_head(cidh));
	}
	
	key = 1;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 1;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 5;
	check(chh_int_dbl_lookup(cidh, &key));
	check(2 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 9);
		check((double)(*cidh_val) == ins_val_make(9));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(1 == chhid_pool_head(cidh));
	}
	
	key = 1;
	val = ins_val_make(key);
	chh_int_dbl_insert(cidh, &key, &val);
	check(3 == chh_int_dbl_elems(cidh));
	
	// 0 = x
	// 1 = 9 -> 1 -> 5 -> x
	
	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 9);
		check((double)(*cidh_val) == ins_val_make(9));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(1 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 1) == 1);
		check(chhid_get_pool_next(cidh, 1) == 0);
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(2 == chhid_pool_head(cidh));
	}
	
	// repeat; remove from tail
	
	key = 5;
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	key = 5;
	check(!chh_int_dbl_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 9 -> 1 -> x
	
	key = 9;
	check(chh_int_dbl_lookup(cidh, &key));
	key = 1;
	check(chh_int_dbl_lookup(cidh, &key));
	check(2 == chh_int_dbl_elems(cidh));

	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 9);
		check((double)(*cidh_val) == ins_val_make(9));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(1 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 1) == 1);
		check(chhid_get_pool_next(cidh, 1) == NO_NEXT_N);
		
		check(0 == chhid_pool_head(cidh));
	}
	
	key = 5;
	val = ins_val_make(key);
	chh_int_dbl_insert(cidh, &key, &val);
	check(3 == chh_int_dbl_elems(cidh));
	
	// 0 = x
	// 1 = 9 -> 5 -> 1 -> x
	
	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 9);
		check((double)(*cidh_val) == ins_val_make(9));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 5);
		check((double)(*cidh_val) == ins_val_make(5));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
		
		tbl_node node = chhid_get_tbl_node_at(cidh, 0);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 2);
		check(NOT_INIT == node.next);
		node = chhid_get_tbl_node_at(cidh, 3);
		check(NOT_INIT == node.next);
		
		node = chhid_get_tbl_node_at(cidh, 1);
		check(9 == node.key);
		check(ins_val_make(9) == node.val);
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == 1);
		check(chhid_get_pool_val(cidh, 1) == 1);
		check(chhid_get_pool_next(cidh, 1) == NO_NEXT_N);
		
		check(2 == chhid_pool_head(cidh));
	}
	
	chh_int_dbl_destroy(cidh);
	return true;
}

static bool test_chh_int_dbl_remove_reinsert_trivial(void)
{
	chh_int_dbl cidh_, * cidh = &cidh_;
	
	check(chh_int_dbl_create_sz(cidh, get_ihash, icmp, 4) == cidh);
		
	tbl_node node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 1);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	int key = 1;
	double val = ins_val_make(key);
	
	check(chh_int_dbl_insert(cidh, &key, &val) == cidh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(chh_int_dbl_elems(cidh) == 1);
	check(!chh_int_dbl_is_empty(cidh));
	
	
	{
		chh_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chh_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check((int)(*cidh_key) == 1);
		check((double)(*cidh_val) == ins_val_make(1));
		cidh_key = chh_int_dbl_iterate(cidh, chhid_iter, &cidh_val);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
	}
	
	node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(ins_val_make(1) == node.val);
	check(NO_NEXT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	check(chh_int_dbl_remove(cidh, &key) == cidh);
	check(chh_int_dbl_is_empty(cidh));
	
	node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 1);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	chh_int_dbl_destroy(cidh);
	return true;
}

static bool test_chh_int_dbl_remove(void)
{
	check(test_chh_int_dbl_remove_from_front());
	check(test_chh_int_dbl_remove_from_tail());
	check(test_chh_int_dbl_remove_from_middle());
	check(test_chh_int_dbl_remove_reinsert_trivial());
	check(test_chh_int_dbl_remove_reinsert_list());
	return true;
}

#include "test_ch_hash_types.h"
// CH_HASH_DEFINE(pint_pdbl, int *, double *);

static bool test_chh_ptrs(void)
{
	chh_pint_pdbl tbl_, * tbl = &tbl_;
	
	check(chh_pint_pdbl_create_sz(tbl, get_ihash, icmp, 4) == tbl);
	
	int foo = 0;
	const int * pfoo = &foo;
	
	double bar = 0.0;
	const double * pbar = &bar;
	
	check(chh_pint_pdbl_insert(tbl, &pfoo, &pbar) == tbl);
	check(*chh_pint_pdbl_lookup(tbl, &pfoo) == pbar);
	
	chh_pint_pdbl_destroy(tbl);
	return true;
}

typedef struct pool_node {
	size_t n;
	void * next;
} pool_node;


static void * chht_priv_pool_reach(chh_priv_pool * pool, size_t n)
{
	return chh_priv_get_elem_addr(pool, n);
}
static size_t chht_priv_pool_index(chh_priv_pool * pool, const void * elem)
{
	chh_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return chhv_get_pool_index(tbl, elem);
}
static size_t chht_priv_pool_get_next(chh_priv_pool * pool, size_t n)
{
	chh_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return chhv_get_pool_next(tbl, n);
}
static size_t chht_priv_pool_head(chh_priv_pool * pool)
{
	chh_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return chhv_pool_head(tbl);
}

static bool test_ch_hash_priv_pool(void)
{
	chh_priv_pool pool_, * pool = &pool_;
	
	check(chh_priv_pool_create(pool, sizeof(size_t), 2) == pool);
	check(chh_priv_pool_elem_size(pool) == sizeof(size_t)*2);
	check(2 == chh_priv_pool_curr_cap(pool));
	check(0 == chht_priv_pool_head(pool));
	
	pool_node * node = NULL;
	
	node = (pool_node *)chht_priv_pool_reach(pool, 0);
	check(1 == chht_priv_pool_index(pool, node->next));
	check(NO_NEXT_N == chht_priv_pool_get_next(pool, 1));
	
	node = (pool_node *)chht_priv_pool_reach(pool, 1);
	check(NULL == node->next);
	check(NO_NEXT_N == chht_priv_pool_index(pool, node->next));
	
	void * elem_a = NULL;
	size_t elem_a_ind = NO_NEXT_N;
	elem_a = chh_priv_pool_get(pool);
	elem_a_ind = chht_priv_pool_index(pool, elem_a);
	check(0 == elem_a_ind);
	check(1 == chht_priv_pool_head(pool));
	*(size_t *)chht_priv_pool_reach(pool, elem_a_ind) = 100;
	check(100 == *(size_t *)chht_priv_pool_reach(pool, elem_a_ind));
	
	void * elem_b = NULL;
	size_t elem_b_ind = NO_NEXT_N;
	elem_b = chh_priv_pool_get(pool);
	elem_b_ind = chht_priv_pool_index(pool, elem_b);
	check(1 == elem_b_ind);
	check(NO_NEXT_N == chht_priv_pool_head(pool));
	*(size_t *)chht_priv_pool_reach(pool, elem_b_ind) = 200;
	check(200 == *(size_t *)chht_priv_pool_reach(pool, elem_b_ind));
	
	void * elem_c = chh_priv_pool_get(pool);
	size_t elem_c_ind = chht_priv_pool_index(pool, elem_c);
	check(0 == elem_c_ind);
	check(2 == chh_priv_pool_curr_cap(pool));
	check(1 == chht_priv_pool_head(pool));
	*(size_t *)chht_priv_pool_reach(pool, elem_c_ind) = 300;
	check(300 == *(size_t *)chht_priv_pool_reach(pool, elem_c_ind));
	
	// 0 is the first element of the second slab, cause the second slab is
	// pushed at the front of the pool list
	// 1 is the second element of the second slab;
	// undefined, since nothing has been written there
	// 2 is the first element of the first slab
	// 3 is the second element of the first slab
	
	check(300 == *(size_t *)chht_priv_pool_reach(pool, 0));
	check(100 == *(size_t *)chht_priv_pool_reach(pool, 2));
	check(200 == *(size_t *)chht_priv_pool_reach(pool, 3));
	
	chh_priv_pool_destroy(pool);
	return true;
}

static ftest tests[] = {
	test_chh_void_create,
	test_chh_void_insert,
	test_chh_void_iterate,
	test_chh_void_rewrite,
	test_chh_void_lookup,
	test_chh_void_clear,
	test_chh_void_copy,
	test_chh_void_remove,
	test_chh_int_dbl_create,
	test_chh_int_dbl_insert,
	test_chh_int_dbl_iterate,
	test_chh_int_dbl_rewrite,
	test_chh_int_dbl_lookup,
	test_chh_int_dbl_clear,
	test_chh_int_dbl_copy,
	test_chh_int_dbl_remove,
	test_chh_ptrs,
	test_ch_hash_priv_pool
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
