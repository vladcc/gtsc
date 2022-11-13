#include <stdio.h>
#include <string.h>
#include "test.h"

#include "ptr_hash/ptr_hash.h"

#define NO_NEXT   NULL
#define NO_NEXT_N ((size_t)~0)
#define NOT_INIT  ptrh_priv_not_init_val()
#define UNUSED(x) (void)(x)

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

static ptrh_compar ptrhv_get_cmp(ptrh_void * chhv)
{
	return chhv->cmp;
}
static ptrh_get_hash ptrhv_get_hash(ptrh_void * chhv)
{
	return chhv->hash;
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

static int ptrhv_get_pool_val(ptrh_void * chhv, size_t n)
{
	const ptrh_node * elem = ptrh_priv_get_elem_addr(&chhv->chain_pool, n);
	return *((int *)elem->key);
}
static size_t ptrhv_get_pool_index(ptrh_void * chhv, const void * elem)
{
	if (!elem)
		goto done;
		
	ptrh_priv_pool * pool = &chhv->chain_pool;

	ptrh_node * p = NULL;
	size_t count = 0;
	while ((p = ptrh_priv_get_elem_addr(pool, count)))
	{
		if (elem == p)
			return count;
		++count;
	}
	
done:
	return NO_NEXT_N;
}
static size_t ptrhv_get_pool_next(ptrh_void * chhv, size_t n)
{
	ptrh_priv_pool * pool = &chhv->chain_pool;
	return ptrhv_get_pool_index(chhv,
		ptrh_priv_pool_next_get(pool,
			ptrh_priv_get_elem_addr(pool, n)));
}
static size_t ptrhv_pool_head(ptrh_void * chhv)
{
	return ptrhv_get_pool_index(chhv,
		ptrh_priv_pool_get_head(&chhv->chain_pool));
}

typedef ptrh_node tbl_node;
static tbl_node * ptrhv_get_tbl(ptrh_void * chhv)
{
	return chhv->data;
}
static bool ptrhv_is_tbl_clear(ptrh_void * chhv)
{
	tbl_node * tbl = ptrhv_get_tbl(chhv);
	for (size_t i = 0, end = ptrh_void_cap(chhv); i < end; ++i)
	{
		if (ptrh_priv_not_init_val() != tbl[i].next)
			return false;
	}
	return true;
}
tbl_node ptrhv_get_tbl_node_at(ptrh_void * chhv, size_t ind)
{
	tbl_node * tbl = ptrhv_get_tbl(chhv);
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
static bool ptrhv_test_copy(ptrh_void * chhv)
{
	node_arr arr_tbl;
	memset(&arr_tbl, 0, sizeof(arr_tbl));
	node_arr arr_cpy;
	memset(&arr_cpy, 0, sizeof(arr_cpy));
	
	ptrh_void cpy_, * cpy = &cpy_;
	bool is_ok = false;
	*cpy = ptrh_void_copy(chhv, &is_ok);
	check(is_ok);
	
	check(ptrh_void_elems(chhv) == ptrh_void_elems(cpy));
	check(ptrh_priv_pool_curr_cap(&chhv->chain_pool)
		== ptrh_priv_pool_curr_cap(&cpy->chain_pool));
	
	ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
	ptrh_iterator cpy_iter_, * cpy_iter = &cpy_iter_;
	
	ptrh_iterator_init(ptrhv_iter);
	ptrh_iterator_init(cpy_iter);
	const void * cvh_key = NULL;
	void * cvh_val = NULL;
	const void * cpy_key = NULL;
	void * cpy_val = NULL;
	
	size_t count = 0;
	while ((cvh_key = ptrh_void_iterate(chhv, ptrhv_iter, &cvh_val)))
	{
		cpy_key = ptrh_void_iterate(cpy, cpy_iter, &cpy_val);
		
		arr_tbl.nodes[count].key = *(int *)cvh_key;
		arr_tbl.nodes[count].val = *(double *)cvh_val;
		
		arr_cpy.nodes[count].key = *(int *)cpy_key;
		arr_cpy.nodes[count].val = *(double *)cpy_val;
		
		++count;
	}
	
	arr_tbl.len = arr_cpy.len = count;
	check(ptrh_void_elems(chhv) == count);
	
	check(node_arr_eq(&arr_tbl, &arr_cpy));
	
	cpy_key = ptrh_void_iterate(cpy, cpy_iter, &cpy_val);
	check(NULL == cvh_key);
	check(NULL == cpy_key);
	check(cvh_key == cpy_key);
	
	check(NULL == cvh_val);
	check(NULL == cpy_val);
	check(cvh_val == cpy_val);
	
	ptrh_void_destroy(cpy);
	return true;
}

static bool test_ptrh_void_create(void)
{
	zero_icmp_calls();
	zero_hash_calls();
	
	{
		ptrh_void cvh_, * cvh = &cvh_;
		check(ptrh_void_create_sz_pl(cvh, get_ihash, icmp, 3, 5) == cvh);
		
		check(5 == ptrh_priv_pool_curr_cap(&cvh->chain_pool));
		check(0 == cvh->elems);
		check(ptrh_void_elems(cvh) == cvh->elems);
		check(ptrh_void_is_empty(cvh));
		check(ptrh_void_cap(cvh) == 4);
		
		check(ptrhv_is_tbl_clear(cvh));
		check(ptrhv_get_tbl(cvh));
		check(ptrhv_get_cmp(cvh) == icmp);
		check(ptrhv_get_hash(cvh) == get_ihash);
		
		check(ptrhv_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		ptrh_void_destroy(cvh);
	}
	
	{
		ptrh_void cvh_, * cvh = &cvh_;
		check(ptrh_void_create(cvh, get_ihash, icmp) == cvh);
		
		check(PTR_HASH_DEFAULT_POOL
			== ptrh_priv_pool_curr_cap(&cvh->chain_pool));
		check(0 == cvh->elems);
		check(ptrh_void_elems(cvh) == cvh->elems);
		check(ptrh_void_is_empty(cvh));
		check(ptrh_void_cap(cvh) == PTR_HASH_DEFAULT_SIZE);
			
		check(ptrhv_is_tbl_clear(cvh));
		check(ptrhv_get_tbl(cvh));
		check(ptrhv_get_cmp(cvh) == icmp);
		check(ptrhv_get_hash(cvh) == get_ihash);
		
		check(ptrhv_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		ptrh_void_destroy(cvh);
	}
	
	return true;
}

#define ins_val_make(x) ((double)((x)*10+2.0)) // some specific bit pattern

static bool test_ptrh_void_insert(void)
{
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	int key_1 = 1;
	double val_1 = ins_val_make(key_1);
	
	check(ptrh_void_insert(cvh, &key_1, &val_1) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(ptrh_void_elems(cvh) == 1);
	check(!ptrh_void_is_empty(cvh));
	
	check(icmp_calls(0));
	check(hash_calls(1));
	
	tbl_node node = ptrhv_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(&val_1 == node.val);
	check(ins_val_make(1) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	int key_2 = 5;
	double val_2 = ins_val_make(key_2);
	check(ptrh_void_insert(cvh, &key_2, &val_2) == cvh);
	// 0 = x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(ptrh_void_elems(cvh) == 2);
	
	check(icmp_calls(1));
	check(hash_calls(2));
	
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(&val_1 == node.val);
	check(ins_val_make(1) == *(double *)node.val);
	check(0 == ptrhv_get_pool_index(cvh, node.next));
	
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key_3 = 4;
	double val_3 = ins_val_make(key_3);
	check(ptrh_void_insert(cvh, &key_3, &val_3) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(ptrh_void_elems(cvh) == 3);
	
	check(icmp_calls(1));
	check(hash_calls(3));
	
	node = ptrhv_get_tbl_node_at(cvh, 0);
	check(&key_3 == node.key);
	check(4 == *(int *)node.key);
	check(&val_3 == node.val);
	check(ins_val_make(4) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	int key_4 = 7;
	double val_4 = ins_val_make(key_4);
	check(ptrh_void_insert(cvh, &key_4, &val_4) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = 7 -> x
	check(ptrh_void_elems(cvh) == 4);
	
	check(icmp_calls(1));
	check(hash_calls(4));
	
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(&key_4 == node.key);
	check(7 == *(int *)node.key);
	check(&val_4 == node.val);
	check(ins_val_make(7) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	
	// growth event
	int key_5 = 12;
	double val_5 = ins_val_make(key_5);
	check(ptrh_void_cap(cvh) == 4);
	check(ptrh_void_insert(cvh, &key_5, &val_5) == cvh);
	check(ptrh_void_cap(cvh) == 8);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(ptrh_void_elems(cvh) == 5);
	
	check(icmp_calls(2));
	check(hash_calls(9));
	
	int key_6 = 20;
	double val_6 = ins_val_make(key_6);
	check(ptrh_void_insert(cvh, &key_6, &val_6) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 20 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(ptrh_void_elems(cvh) == 6);
	
	check(icmp_calls(4));
	check(hash_calls(10));
	
	node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(&val_1 == node.val);
	check(ins_val_make(1) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 4);
	check(&key_3 == node.key);
	check(4 == *(int *)node.key);
	check(&val_3 == node.val);
	check(ins_val_make(4) == *(double *)node.val);
	check(1 == ptrhv_get_pool_index(cvh, node.next));
	
	check(ptrhv_get_pool_val(cvh, 1) == 20);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 12);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	node = ptrhv_get_tbl_node_at(cvh, 5);
	check(&key_2 == node.key);
	check(5 == *(int *)node.key);
	check(&val_2 == node.val);
	check(ins_val_make(5) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 6);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 7);
	check(&key_4 == node.key);
	check(7 == *(int *)node.key);
	check(&val_4 == node.val);
	check(ins_val_make(7) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	ptrh_void_destroy(cvh);
	
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	return true;
}

typedef struct data_node {
	int key;
	double val;
} data_node;
static ptrh_void * ptrhv_ins(ptrh_void * cvh, data_node * arr, size_t len)
{
	for (size_t i = 0; i < len; ++i)
		ptrh_void_insert(cvh, &arr[i].key, &arr[i].val);
	return cvh;
}
static bool ptrhv_get4(ptrh_void * cvh)
{
	static data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{4, ins_val_make(4)},
		{7, ins_val_make(7)},
	};
	
	ptrhv_ins(cvh, arr, 4);
	check(4 == ptrh_void_elems(cvh));
	check(!ptrh_void_is_empty(cvh));
	return true;
}
static bool ptrhv_get6(ptrh_void * cvh)
{
	static data_node arr[] = {
		{12, ins_val_make(12)},
		{20, ins_val_make(20)},
	};
	
	ptrhv_get4(cvh);
	ptrhv_ins(cvh, arr, 2);
	check(6 == ptrh_void_elems(cvh));
	check(!ptrh_void_is_empty(cvh));
	return true;
}

static bool test_ptrh_void_iterate(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhv_get4(cvh);
	
	ptrh_iterator iter_, * iter = &iter_;
	ptrh_iterator_init(iter);
	
	const void * key = NULL;
	void * val = NULL;
	
	key = ptrh_void_iterate(cvh, iter, &val);
	check(4 == *(int *)key && ins_val_make(4) == *(double *)val);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(1 == *(int *)key && ins_val_make(1) == *(double *)val);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(5 == *(int *)key && ins_val_make(5) == *(double *)val);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(7 == *(int *)key && ins_val_make(7) == *(double *)val);
	
	key = ptrh_void_iterate(cvh, iter, &val);
	check(NULL == key);
	check(NULL == val);
	
	ptrhv_get6(cvh);
	
	ptrh_iterator_init(iter);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(1 == *(int *)key && ins_val_make(1) == *(double *)val);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(4 == *(int *)key && ins_val_make(4) == *(double *)val);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(20 == *(int *)key && ins_val_make(20) == *(double *)val);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(12 == *(int *)key && ins_val_make(12) == *(double *)val);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(5 == *(int *)key && ins_val_make(5) == *(double *)val);
	key = ptrh_void_iterate(cvh, iter, &val);
	check(7 == *(int *)key && ins_val_make(7) == *(double *)val);
	
	key = ptrh_void_iterate(cvh, iter, &val);
	check(NULL == key);
	check(NULL == val);
	
	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_rewrite(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhv_get4(cvh);
	
	int key_1 = 1;
	double val_1 = ins_val_make(key_1*10);
	check(ptrh_void_insert(cvh, &key_1, &val_1) == cvh);
	
	int key_2 = 4;
	double val_2 = ins_val_make(key_2*10);
	check(ptrh_void_insert(cvh, &key_2, &val_2) == cvh);
	
	int key_3 = 5;
	double val_3 = ins_val_make(key_3*10);
	check(ptrh_void_insert(cvh, &key_3, &val_3) == cvh);
		
	ptrh_iterator iter_, * iter = &iter_;
	ptrh_iterator_init(iter);
	
	const void * pkey = NULL;
	void * pval = NULL;
	
	pkey = ptrh_void_iterate(cvh, iter, &pval);
	check(4 == *(int *)pkey && ins_val_make(40) == *(double *)pval);
	pkey = ptrh_void_iterate(cvh, iter, &pval);
	check(1 == *(int *)pkey && ins_val_make(10) == *(double *)pval);
	pkey = ptrh_void_iterate(cvh, iter, &pval);
	check(5 == *(int *)pkey && ins_val_make(50) == *(double *)pval);
	pkey = ptrh_void_iterate(cvh, iter, &pval);
	check(7 == *(int *)pkey && ins_val_make(7) == *(double *)pval);
	
	pkey = ptrh_void_iterate(cvh, iter, &pval);
	check(NULL == pkey);
	check(NULL == pval);
		
	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_lookup(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	int key = 4;
	const void * pval = NULL;
	check(NULL == ptrh_void_lookup(cvh, &key));
	
	key = 0;
	check(NULL == ptrh_void_lookup(cvh, &key));
	
	ptrhv_get4(cvh);
	key = 4;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1000;
	check(NULL == ptrh_void_lookup(cvh, &key));
	
	key = 5;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = -1000;
	check(NULL == ptrh_void_lookup(cvh, &key));
	
	ptrhv_get6(cvh);
	
	key = 5;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 20;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_clear(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhv_get4(cvh);
	
	int key = 4;
	const void * pval = NULL;
	
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 5;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	size_t h_calls = get_hash_calls();
	size_t cmp_calls = get_icmp_calls();
	check(ptrh_void_clear(cvh) == cvh);
	check(0 == ptrh_void_elems(cvh));
	check(ptrh_void_is_empty(cvh));
	hash_calls(h_calls);
	icmp_calls(cmp_calls);
	
	key = 4;
	pval = ptrh_void_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 1;
	pval = ptrh_void_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 5;
	pval = ptrh_void_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 7;
	pval = ptrh_void_lookup(cvh, &key);
	check(NULL == pval);
	
	ptrhv_get6(cvh);
	
	key = 1;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 5;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 4;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 20;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 12;
	pval = ptrh_void_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_copy(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	check(ptrhv_test_copy(cvh));
	
	ptrhv_get4(cvh);
	check(ptrhv_test_copy(cvh));
	
	ptrhv_get6(cvh);
	check(ptrhv_test_copy(cvh));
	
	ptrh_void_destroy(cvh);
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 5) == cvh);
	
	ptrhv_get6(cvh);
	check(ptrhv_test_copy(cvh));
	
	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_remove_from_front(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{9, ins_val_make(9)},
	};

	ptrhv_ins(cvh, arr, 3);
	check(3 == ptrh_void_elems(cvh));
	check(!ptrh_void_is_empty(cvh));
	check(4 == ptrh_void_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(1 == ptrhv_get_pool_index(cvh, node.next));
	
	check(ptrhv_get_pool_val(cvh, 1) == 9);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(ptrh_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrh_void_lookup(cvh, &key));
	key = 5;
	check(ptrh_void_lookup(cvh, &key));
	key = 9;
	check(ptrh_void_lookup(cvh, &key));
	check(3 == ptrh_void_elems(cvh));
	
	key = 1;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(ptrh_void_lookup(cvh, &key));
	key = 5;
	check(ptrh_void_lookup(cvh, &key));
	check(2 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh,0) == NO_NEXT_N);
		
		check(1 == ptrhv_pool_head(cvh));
	}
	
	key = 9;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(ptrh_void_lookup(cvh, &key));
	check(1 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(5 == *(int *)node.key);
		check(ins_val_make(5) == *(double *)node.val);
		check(NO_NEXT == node.next);
		
		check(0 == ptrhv_pool_head(cvh));
	}

	key = 5;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrh_void_elems(cvh));
	check(ptrh_void_is_empty(cvh));
	
	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(0 == ptrhv_pool_head(cvh));
	}

	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_remove_from_tail(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{9, ins_val_make(9)},
	};
	
	ptrhv_ins(cvh, arr, 3);
	check(3 == ptrh_void_elems(cvh));
	check(!ptrh_void_is_empty(cvh));
	check(4 == ptrh_void_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(1 == ptrhv_get_pool_index(cvh, node.next));

	check(ptrhv_get_pool_val(cvh, 1) == 9);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(ptrh_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrh_void_lookup(cvh, &key));
	key = 5;
	check(ptrh_void_lookup(cvh, &key));
	key = 9;
	check(ptrh_void_lookup(cvh, &key));
	check(3 == ptrh_void_elems(cvh));
	
	key = 5;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 9 -> x
	
	key = 1;
	check(ptrh_void_lookup(cvh, &key));
	key = 9;
	check(ptrh_void_lookup(cvh, &key));
	check(2 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(1 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 1) == 9);
		check(ptrhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == ptrhv_pool_head(cvh));
	}
	
	key = 9;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> x
	
	key = 1;
	check(ptrh_void_lookup(cvh, &key));
	check(1 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(NO_NEXT == node.next);
		
		check(1 == ptrhv_pool_head(cvh));
	}

	key = 1;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrh_void_elems(cvh));
	check(ptrh_void_is_empty(cvh));
	
	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(1 == ptrhv_pool_head(cvh));
	}

	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_remove_from_middle(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{9, ins_val_make(9)},
	};
	
	ptrhv_ins(cvh, arr, 3);
	check(3 == ptrh_void_elems(cvh));
	check(!ptrh_void_is_empty(cvh));
	check(4 == ptrh_void_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(1 == ptrhv_get_pool_index(cvh, node.next));

	check(ptrhv_get_pool_val(cvh, 1) == 9);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key = 10;
	check(ptrh_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrh_void_lookup(cvh, &key));
	key = 9;
	check(ptrh_void_lookup(cvh, &key));
	key = 5;
	check(ptrh_void_lookup(cvh, &key));
	check(3 == ptrh_void_elems(cvh));
	
	key = 9;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(ptrh_void_lookup(cvh, &key));
	key = 5;
	check(ptrh_void_lookup(cvh, &key));
	check(2 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhv_pool_head(cvh));
	}
	
	key = 1;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(ptrh_void_lookup(cvh, &key));
	check(1 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(5 == *(int *)node.key);
		check(ins_val_make(5) == *(double *)node.val);
		check(NO_NEXT == node.next);
		
		check(0 == ptrhv_pool_head(cvh));
	}

	key = 5;
	check(ptrh_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrh_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrh_void_elems(cvh));
	check(ptrh_void_is_empty(cvh));
	
	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(0 == ptrhv_pool_head(cvh));
	}

	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_remove_reinsert_list(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{9, ins_val_make(9)},
	};
	
	ptrhv_ins(cvh, arr, 3);
	check(3 == ptrh_void_elems(cvh));
	check(!ptrh_void_is_empty(cvh));
	check(4 == ptrh_void_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(1 == ptrhv_get_pool_index(cvh, node.next));

	check(ptrhv_get_pool_val(cvh, 1) == 9);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key_0 = 10;
	check(ptrh_void_remove(cvh, &key_0) == cvh);
	
	key_0 = 1;
	check(ptrh_void_lookup(cvh, &key_0));
	key_0 = 5;
	check(ptrh_void_lookup(cvh, &key_0));
	key_0 = 9;
	check(ptrh_void_lookup(cvh, &key_0));
	check(3 == ptrh_void_elems(cvh));
	
	key_0 = 9;
	check(ptrh_void_remove(cvh, &key_0) == cvh);
	key_0 = 9;
	check(!ptrh_void_lookup(cvh, &key_0));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key_0 = 1;
	check(ptrh_void_lookup(cvh, &key_0));
	key_0 = 5;
	check(ptrh_void_lookup(cvh, &key_0));
	check(2 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhv_pool_head(cvh));
	}
	
	int key_1 = 9;
	double val_1 = ins_val_make(key_1);
	ptrh_void_insert(cvh, &key_1, &val_1);
	check(3 == ptrh_void_elems(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	
	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(1 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 1) == 9);
		check(ptrhv_get_pool_next(cvh, 1) == 0);
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == ptrhv_pool_head(cvh));
	}
	
	int key_2 = 1;
	check(ptrh_void_remove(cvh, &key_2) == cvh);
	key_2 = 1;
	check(!ptrh_void_lookup(cvh, &key_2));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key_2 = 9;
	check(ptrh_void_lookup(cvh, &key_2));
	key_2 = 5;
	check(ptrh_void_lookup(cvh, &key_2));
	check(2 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhv_pool_head(cvh));
	}
	
	int key_3 = 1;
	double val_3 = ins_val_make(key_3);
	ptrh_void_insert(cvh, &key_3, &val_3);
	check(3 == ptrh_void_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 1 -> 5 -> x
	
	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(1 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 1) == 1);
		check(ptrhv_get_pool_next(cvh, 1) == 0);
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == ptrhv_pool_head(cvh));
	}
	
	// repeat; remove from tail
	
	int key_4 = 5;
	check(ptrh_void_remove(cvh, &key_4) == cvh);
	key_4 = 5;
	check(!ptrh_void_lookup(cvh, &key_4));
	
	// 0 = x
	// 1 = 9 -> 1 -> x
	
	key_4 = 9;
	check(ptrh_void_lookup(cvh, &key_4));
	key_4 = 1;
	check(ptrh_void_lookup(cvh, &key_4));
	check(2 == ptrh_void_elems(cvh));

	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(1 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 1) == 1);
		check(ptrhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == ptrhv_pool_head(cvh));
	}
	
	int key_5 = 5;
	double val_5 = ins_val_make(key_5);
	ptrh_void_insert(cvh, &key_5, &val_5);
	check(3 == ptrh_void_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 5 -> 1 -> x
	
	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhv_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhv_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == 1);
		check(ptrhv_get_pool_val(cvh, 1) == 1);
		check(ptrhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(2 == ptrhv_pool_head(cvh));
	}
	
	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_remove_reinsert_trivial(void)
{
	ptrh_void cvh_, * cvh = &cvh_;
	
	check(ptrh_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	int key = 1;
	double val = ins_val_make(key);
	
	check(ptrh_void_insert(cvh, &key, &val) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(ptrh_void_elems(cvh) == 1);
	check(!ptrh_void_is_empty(cvh));
	
	
	{
		ptrh_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrh_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_void_iterate(cvh, ptrhv_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
	}
	
	node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(NO_NEXT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	check(ptrh_void_remove(cvh, &key) == cvh);
	check(ptrh_void_is_empty(cvh));
	
	node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	ptrh_void_destroy(cvh);
	return true;
}

static bool test_ptrh_void_remove(void)
{
	check(test_ptrh_void_remove_from_front());
	check(test_ptrh_void_remove_from_tail());
	check(test_ptrh_void_remove_from_middle());
	check(test_ptrh_void_remove_reinsert_trivial());
	check(test_ptrh_void_remove_reinsert_list());
	return true;
}

PTR_HASH_DEFINE(int_dbl, int, double);

static int ptrhid_get_pool_val(ptrh_int_dbl * ptrhid, size_t n)
{
	return ptrhv_get_pool_val(&ptrhid->tbl, n);
}
static size_t ptrhid_get_pool_next(ptrh_int_dbl * ptrhid, size_t n)
{
	return ptrhv_get_pool_next(&ptrhid->tbl, n);
}
static size_t ptrhid_get_pool_index(ptrh_int_dbl * ptrhid, const void * elem)
{
	return ptrhv_get_pool_index(&ptrhid->tbl, elem);
}
static void * ptrhid_get_tbl(ptrh_int_dbl * ptrhid)
{
	return ptrhv_get_tbl(&ptrhid->tbl);
}
static size_t ptrhid_pool_head(ptrh_int_dbl * ptrhid)
{
	return ptrhv_pool_head(&ptrhid->tbl);
}
static ptrh_compar ptrhid_get_cmp(ptrh_int_dbl * ptrhid)
{
	return ptrhv_get_cmp(&ptrhid->tbl);
}
static ptrh_get_hash ptrhid_get_hash(ptrh_int_dbl * ptrhid)
{
	return ptrhv_get_hash(&ptrhid->tbl);
}

static bool ptrhid_is_tbl_clear(ptrh_int_dbl * ptrhid)
{
	return ptrhv_is_tbl_clear(&ptrhid->tbl);
}
tbl_node ptrhid_get_tbl_node_at(ptrh_int_dbl * ptrhid, size_t ind)
{
	return ptrhv_get_tbl_node_at(&ptrhid->tbl, ind);
}
static bool ptrhid_test_copy(ptrh_int_dbl * ptrhid)
{
	check(ptrhv_test_copy(&ptrhid->tbl));
	return true;
}

static bool test_ptrh_int_dbl_create(void)
{
	zero_icmp_calls();
	zero_hash_calls();
	
	{
		ptrh_int_dbl cvh_, * cvh = &cvh_;
		check(ptrh_int_dbl_create_sz_pl(cvh, get_ihash, icmp, 3, 5) == cvh);
		
		check(ptrh_priv_pool_curr_cap(&cvh->tbl.chain_pool) == 5);
		check(0 == cvh->tbl.elems);
		check(ptrh_int_dbl_elems(cvh) == cvh->tbl.elems);
		check(ptrh_int_dbl_is_empty(cvh));
		check(ptrh_int_dbl_cap(cvh) == 4);
		
		check(ptrhid_is_tbl_clear(cvh));
		check(ptrhid_get_tbl(cvh));
		check(ptrhid_get_cmp(cvh) == icmp);
		check(ptrhid_get_hash(cvh) == get_ihash);
		
		check(ptrhid_test_copy(cvh));
		
		{
			// use named copy function
			bool is_ok = false;
			ptrh_int_dbl copy_, * copy = &copy_;
			
			*copy = ptrh_int_dbl_copy(cvh, &is_ok);
			check(is_ok);
			
			check(ptrh_priv_pool_curr_cap(&copy->tbl.chain_pool) == 5);
			check(0 == copy->tbl.elems);
			check(ptrh_int_dbl_elems(copy) == copy->tbl.elems);
			check(ptrh_int_dbl_is_empty(copy));
			check(ptrh_int_dbl_cap(copy) == 1);
			
			check(ptrhid_is_tbl_clear(copy));
			check(ptrhid_get_tbl(copy));
			check(ptrhid_get_cmp(copy) == icmp);
			check(ptrhid_get_hash(copy) == get_ihash);
			
			ptrh_int_dbl_destroy(copy);
		}
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		ptrh_int_dbl_destroy(cvh);
	}
	
	{
		ptrh_int_dbl cvh_, * cvh = &cvh_;
		check(ptrh_int_dbl_create(cvh, get_ihash, icmp) == cvh);
		
		check(0 == cvh->tbl.elems);
		check(ptrh_int_dbl_elems(cvh) == cvh->tbl.elems);
		check(ptrh_int_dbl_is_empty(cvh));
		check(ptrh_int_dbl_cap(cvh) == 32);
			
		check(ptrhid_is_tbl_clear(cvh));
		check(ptrhid_get_tbl(cvh));
		check(ptrhid_get_cmp(cvh) == icmp);
		check(ptrhid_get_hash(cvh) == get_ihash);
		
		check(ptrhid_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		ptrh_int_dbl_destroy(cvh);
	}
	
	return true;
}

#define ins_val_make(x) ((double)((x)*10+2.0)) // some specific bit pattern

static bool test_ptrh_int_dbl_insert(void)
{
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	int key_1 = 1;
	double val_1 = ins_val_make(key_1);
	
	check(ptrh_int_dbl_insert(cvh, &key_1, &val_1) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(ptrh_int_dbl_elems(cvh) == 1);
	check(!ptrh_int_dbl_is_empty(cvh));
	
	check(icmp_calls(0));
	check(hash_calls(1));
	
	tbl_node node = ptrhid_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(&val_1 == node.val);
	check(ins_val_make(1) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	int key_2 = 5;
	double val_2 = ins_val_make(key_2);
	check(ptrh_int_dbl_insert(cvh, &key_2, &val_2) == cvh);
	// 0 = x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(ptrh_int_dbl_elems(cvh) == 2);
	
	check(icmp_calls(1));
	check(hash_calls(2));
	
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(&val_1 == node.val);
	check(ins_val_make(1) == *(double *)node.val);
	check(0 == ptrhid_get_pool_index(cvh, node.next));
	
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key_3 = 4;
	double val_3 = ins_val_make(key_3);
	check(ptrh_int_dbl_insert(cvh, &key_3, &val_3) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(ptrh_int_dbl_elems(cvh) == 3);
	
	check(icmp_calls(1));
	check(hash_calls(3));
	
	node = ptrhid_get_tbl_node_at(cvh, 0);
	check(&key_3 == node.key);
	check(4 == *(int *)node.key);
	check(&val_3 == node.val);
	check(ins_val_make(4) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	int key_4 = 7;
	double val_4 = ins_val_make(key_4);
	check(ptrh_int_dbl_insert(cvh, &key_4, &val_4) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = 7 -> x
	check(ptrh_int_dbl_elems(cvh) == 4);
	
	check(icmp_calls(1));
	check(hash_calls(4));
	
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(&key_4 == node.key);
	check(7 == *(int *)node.key);
	check(&val_4 == node.val);
	check(ins_val_make(7) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	
	// growth event
	int key_5 = 12;
	double val_5 = ins_val_make(key_5);
	check(ptrh_int_dbl_cap(cvh) == 4);
	check(ptrh_int_dbl_insert(cvh, &key_5, &val_5) == cvh);
	check(ptrh_int_dbl_cap(cvh) == 8);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(ptrh_int_dbl_elems(cvh) == 5);
	
	check(icmp_calls(2));
	check(hash_calls(9));
	
	int key_6 = 20;
	double val_6 = ins_val_make(key_6);
	check(ptrh_int_dbl_insert(cvh, &key_6, &val_6) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 20 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(ptrh_int_dbl_elems(cvh) == 6);
	
	check(icmp_calls(4));
	check(hash_calls(10));
	
	node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(&val_1 == node.val);
	check(ins_val_make(1) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 4);
	check(&key_3 == node.key);
	check(4 == *(int *)node.key);
	check(&val_3 == node.val);
	check(ins_val_make(4) == *(double *)node.val);
	check(1 == ptrhid_get_pool_index(cvh, node.next));
	
	check(ptrhid_get_pool_val(cvh, 1) == 20);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 12);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	node = ptrhid_get_tbl_node_at(cvh, 5);
	check(&key_2 == node.key);
	check(5 == *(int *)node.key);
	check(&val_2 == node.val);
	check(ins_val_make(5) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 6);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 7);
	check(&key_4 == node.key);
	check(7 == *(int *)node.key);
	check(&val_4 == node.val);
	check(ins_val_make(7) == *(double *)node.val);
	check(NO_NEXT == node.next);
	
	ptrh_int_dbl_destroy(cvh);
	
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	return true;
}

static ptrh_int_dbl * ptrhid_ins(ptrh_int_dbl * cvh, data_node * arr, size_t len)
{
	for (size_t i = 0; i < len; ++i)
		ptrh_int_dbl_insert(cvh, &arr[i].key, &arr[i].val);
	return cvh;
}
static bool ptrhid_get4(ptrh_int_dbl * cvh)
{
	static data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{4, ins_val_make(4)},
		{7, ins_val_make(7)},
	};
	
	ptrhid_ins(cvh, arr, 4);
	check(4 == ptrh_int_dbl_elems(cvh));
	check(!ptrh_int_dbl_is_empty(cvh));
	return true;
}
static bool ptrhid_get6(ptrh_int_dbl * cvh)
{
	static data_node arr[] = {
		{12, ins_val_make(12)},
		{20, ins_val_make(20)},
	};
	
	ptrhid_get4(cvh);
	ptrhid_ins(cvh, arr, 2);
	check(6 == ptrh_int_dbl_elems(cvh));
	check(!ptrh_int_dbl_is_empty(cvh));
	return true;
}

static bool test_ptrh_int_dbl_iterate(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhid_get4(cvh);
	
	ptrh_iterator iter_, * iter = &iter_;
	ptrh_iterator_init(iter);
	
	const int * key = NULL;
	double * val = NULL;
	
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(4 == *(int *)key && ins_val_make(4) == *(double *)val);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(1 == *(int *)key && ins_val_make(1) == *(double *)val);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(5 == *(int *)key && ins_val_make(5) == *(double *)val);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(7 == *(int *)key && ins_val_make(7) == *(double *)val);
	
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(NULL == key);
	check(NULL == val);
	
	ptrhid_get6(cvh);
	
	ptrh_iterator_init(iter);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(1 == *(int *)key && ins_val_make(1) == *(double *)val);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(4 == *(int *)key && ins_val_make(4) == *(double *)val);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(20 == *(int *)key && ins_val_make(20) == *(double *)val);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(12 == *(int *)key && ins_val_make(12) == *(double *)val);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(5 == *(int *)key && ins_val_make(5) == *(double *)val);
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(7 == *(int *)key && ins_val_make(7) == *(double *)val);
	
	key = ptrh_int_dbl_iterate(cvh, iter, &val);
	check(NULL == key);
	check(NULL == val);
	
	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_rewrite(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhid_get4(cvh);
	
	int key_1 = 1;
	double val_1 = ins_val_make(key_1*10);
	check(ptrh_int_dbl_insert(cvh, &key_1, &val_1) == cvh);
	
	int key_2 = 4;
	double val_2 = ins_val_make(key_2*10);
	check(ptrh_int_dbl_insert(cvh, &key_2, &val_2) == cvh);
	
	int key_3 = 5;
	double val_3 = ins_val_make(key_3*10);
	check(ptrh_int_dbl_insert(cvh, &key_3, &val_3) == cvh);
		
	ptrh_iterator iter_, * iter = &iter_;
	ptrh_iterator_init(iter);
	
	const int * pkey = NULL;
	double * pval = NULL;
	
	pkey = ptrh_int_dbl_iterate(cvh, iter, &pval);
	check(4 == *(int *)pkey && ins_val_make(40) == *(double *)pval);
	pkey = ptrh_int_dbl_iterate(cvh, iter, &pval);
	check(1 == *(int *)pkey && ins_val_make(10) == *(double *)pval);
	pkey = ptrh_int_dbl_iterate(cvh, iter, &pval);
	check(5 == *(int *)pkey && ins_val_make(50) == *(double *)pval);
	pkey = ptrh_int_dbl_iterate(cvh, iter, &pval);
	check(7 == *(int *)pkey && ins_val_make(7) == *(double *)pval);
	
	pkey = ptrh_int_dbl_iterate(cvh, iter, &pval);
	check(NULL == pkey);
	check(NULL == pval);
		
	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_lookup(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	int key = 4;
	const double * pval = NULL;
	check(NULL == ptrh_int_dbl_lookup(cvh, &key));
	
	key = 0;
	check(NULL == ptrh_int_dbl_lookup(cvh, &key));
	
	ptrhid_get4(cvh);
	key = 4;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1000;
	check(NULL == ptrh_int_dbl_lookup(cvh, &key));
	
	key = 5;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = -1000;
	check(NULL == ptrh_int_dbl_lookup(cvh, &key));
	
	ptrhid_get6(cvh);
	
	key = 5;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 20;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_clear(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhid_get4(cvh);
	
	int key = 4;
	const double * pval = NULL;
	
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 1;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 5;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	size_t h_calls = get_hash_calls();
	size_t cmp_calls = get_icmp_calls();
	check(ptrh_int_dbl_clear(cvh) == cvh);
	check(0 == ptrh_int_dbl_elems(cvh));
	check(ptrh_int_dbl_is_empty(cvh));
	hash_calls(h_calls);
	icmp_calls(cmp_calls);
	
	key = 4;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 1;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 5;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(NULL == pval);
	
	key = 7;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(NULL == pval);
	
	ptrhid_get6(cvh);
	
	key = 1;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 5;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 7;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 4;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 20;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	key = 12;
	pval = ptrh_int_dbl_lookup(cvh, &key);
	check(*(double *)pval == ins_val_make(key));
	
	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_copy(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	check(ptrhid_test_copy(cvh));
	
	ptrhid_get4(cvh);
	check(ptrhid_test_copy(cvh));
	
	ptrhid_get6(cvh);
	check(ptrhid_test_copy(cvh));
	
	ptrh_int_dbl_destroy(cvh);
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 5) == cvh);
	
	ptrhid_get6(cvh);
	check(ptrhid_test_copy(cvh));
	
	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_remove_from_front(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{9, ins_val_make(9)},
	};

	ptrhid_ins(cvh, arr, 3);
	check(3 == ptrh_int_dbl_elems(cvh));
	check(!ptrh_int_dbl_is_empty(cvh));
	check(4 == ptrh_int_dbl_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(1 == ptrhid_get_pool_index(cvh, node.next));
	
	check(ptrhid_get_pool_val(cvh, 1) == 9);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 5;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 9;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(3 == ptrh_int_dbl_elems(cvh));
	
	key = 1;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 5;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(2 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh,0) == NO_NEXT_N);
		
		check(1 == ptrhid_pool_head(cvh));
	}
	
	key = 9;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(1 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(5 == *(int *)node.key);
		check(ins_val_make(5) == *(double *)node.val);
		check(NO_NEXT == node.next);
		
		check(0 == ptrhid_pool_head(cvh));
	}

	key = 5;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrh_int_dbl_elems(cvh));
	check(ptrh_int_dbl_is_empty(cvh));
	
	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(0 == ptrhid_pool_head(cvh));
	}

	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_remove_from_tail(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{9, ins_val_make(9)},
	};
	
	ptrhid_ins(cvh, arr, 3);
	check(3 == ptrh_int_dbl_elems(cvh));
	check(!ptrh_int_dbl_is_empty(cvh));
	check(4 == ptrh_int_dbl_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(1 == ptrhid_get_pool_index(cvh, node.next));

	check(ptrhid_get_pool_val(cvh, 1) == 9);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 5;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 9;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(3 == ptrh_int_dbl_elems(cvh));
	
	key = 5;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 9 -> x
	
	key = 1;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 9;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(2 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(1 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 1) == 9);
		check(ptrhid_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == ptrhid_pool_head(cvh));
	}
	
	key = 9;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> x
	
	key = 1;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(1 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(NO_NEXT == node.next);
		
		check(1 == ptrhid_pool_head(cvh));
	}

	key = 1;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrh_int_dbl_elems(cvh));
	check(ptrh_int_dbl_is_empty(cvh));
	
	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(1 == ptrhid_pool_head(cvh));
	}

	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_remove_from_middle(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{9, ins_val_make(9)},
	};
	
	ptrhid_ins(cvh, arr, 3);
	check(3 == ptrh_int_dbl_elems(cvh));
	check(!ptrh_int_dbl_is_empty(cvh));
	check(4 == ptrh_int_dbl_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(1 == ptrhid_get_pool_index(cvh, node.next));

	check(ptrhid_get_pool_val(cvh, 1) == 9);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key = 10;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 9;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 5;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(3 == ptrh_int_dbl_elems(cvh));
	
	key = 9;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(ptrh_int_dbl_lookup(cvh, &key));
	key = 5;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(2 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhid_pool_head(cvh));
	}
	
	key = 1;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(ptrh_int_dbl_lookup(cvh, &key));
	check(1 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(5 == *(int *)node.key);
		check(ins_val_make(5) == *(double *)node.val);
		check(NO_NEXT == node.next);
		
		check(0 == ptrhid_pool_head(cvh));
	}

	key = 5;
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrh_int_dbl_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrh_int_dbl_elems(cvh));
	check(ptrh_int_dbl_is_empty(cvh));
	
	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		check(0 == ptrhid_pool_head(cvh));
	}

	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_remove_reinsert_list(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1, ins_val_make(1)},
		{5, ins_val_make(5)},
		{9, ins_val_make(9)},
	};
	
	ptrhid_ins(cvh, arr, 3);
	check(3 == ptrh_int_dbl_elems(cvh));
	check(!ptrh_int_dbl_is_empty(cvh));
	check(4 == ptrh_int_dbl_cap(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	// 2 = x
	// 3 = x
	
	tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(1 == ptrhid_get_pool_index(cvh, node.next));

	check(ptrhid_get_pool_val(cvh, 1) == 9);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key_0 = 10;
	check(ptrh_int_dbl_remove(cvh, &key_0) == cvh);
	
	key_0 = 1;
	check(ptrh_int_dbl_lookup(cvh, &key_0));
	key_0 = 5;
	check(ptrh_int_dbl_lookup(cvh, &key_0));
	key_0 = 9;
	check(ptrh_int_dbl_lookup(cvh, &key_0));
	check(3 == ptrh_int_dbl_elems(cvh));
	
	key_0 = 9;
	check(ptrh_int_dbl_remove(cvh, &key_0) == cvh);
	key_0 = 9;
	check(!ptrh_int_dbl_lookup(cvh, &key_0));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key_0 = 1;
	check(ptrh_int_dbl_lookup(cvh, &key_0));
	key_0 = 5;
	check(ptrh_int_dbl_lookup(cvh, &key_0));
	check(2 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhid_pool_head(cvh));
	}
	
	int key_1 = 9;
	double val_1 = ins_val_make(key_1);
	ptrh_int_dbl_insert(cvh, &key_1, &val_1);
	check(3 == ptrh_int_dbl_elems(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	
	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(1 == *(int *)node.key);
		check(ins_val_make(1) == *(double *)node.val);
		check(1 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 1) == 9);
		check(ptrhid_get_pool_next(cvh, 1) == 0);
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == ptrhid_pool_head(cvh));
	}
	
	int key_2 = 1;
	check(ptrh_int_dbl_remove(cvh, &key_2) == cvh);
	key_2 = 1;
	check(!ptrh_int_dbl_lookup(cvh, &key_2));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key_2 = 9;
	check(ptrh_int_dbl_lookup(cvh, &key_2));
	key_2 = 5;
	check(ptrh_int_dbl_lookup(cvh, &key_2));
	check(2 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhid_pool_head(cvh));
	}
	
	int key_3 = 1;
	double val_3 = ins_val_make(key_3);
	ptrh_int_dbl_insert(cvh, &key_3, &val_3);
	check(3 == ptrh_int_dbl_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 1 -> 5 -> x
	
	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(1 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 1) == 1);
		check(ptrhid_get_pool_next(cvh, 1) == 0);
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == ptrhid_pool_head(cvh));
	}
	
	// repeat; remove from tail
	
	int key_4 = 5;
	check(ptrh_int_dbl_remove(cvh, &key_4) == cvh);
	key_4 = 5;
	check(!ptrh_int_dbl_lookup(cvh, &key_4));
	
	// 0 = x
	// 1 = 9 -> 1 -> x
	
	key_4 = 9;
	check(ptrh_int_dbl_lookup(cvh, &key_4));
	key_4 = 1;
	check(ptrh_int_dbl_lookup(cvh, &key_4));
	check(2 == ptrh_int_dbl_elems(cvh));

	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(1 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 1) == 1);
		check(ptrhid_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == ptrhid_pool_head(cvh));
	}
	
	int key_5 = 5;
	double val_5 = ins_val_make(key_5);
	ptrh_int_dbl_insert(cvh, &key_5, &val_5);
	check(3 == ptrh_int_dbl_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 5 -> 1 -> x
	
	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 9);
		check((double)(*(double *)cvh_val) == ins_val_make(9));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 5);
		check((double)(*(double *)cvh_val) == ins_val_make(5));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
		
		tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 2);
		check(NOT_INIT == node.next);
		node = ptrhid_get_tbl_node_at(cvh, 3);
		check(NOT_INIT == node.next);
		
		node = ptrhid_get_tbl_node_at(cvh, 1);
		check(9 == *(int *)node.key);
		check(ins_val_make(9) == *(double *)node.val);
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == 1);
		check(ptrhid_get_pool_val(cvh, 1) == 1);
		check(ptrhid_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(2 == ptrhid_pool_head(cvh));
	}
	
	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_remove_reinsert_trivial(void)
{
	ptrh_int_dbl cvh_, * cvh = &cvh_;
	
	check(ptrh_int_dbl_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	int key = 1;
	double val = ins_val_make(key);
	
	check(ptrh_int_dbl_insert(cvh, &key, &val) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(ptrh_int_dbl_elems(cvh) == 1);
	check(!ptrh_int_dbl_is_empty(cvh));
	
	
	{
		ptrh_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrh_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check((int)(*(int *)cvh_key) == 1);
		check((double)(*(double *)cvh_val) == ins_val_make(1));
		cvh_key = ptrh_int_dbl_iterate(cvh, ptrhid_iter, &cvh_val);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
	}
	
	node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(ins_val_make(1) == *(double *)node.val);
	check(NO_NEXT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	check(ptrh_int_dbl_remove(cvh, &key) == cvh);
	check(ptrh_int_dbl_is_empty(cvh));
	
	node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	ptrh_int_dbl_destroy(cvh);
	return true;
}

static bool test_ptrh_int_dbl_remove(void)
{
	check(test_ptrh_int_dbl_remove_from_front());
	check(test_ptrh_int_dbl_remove_from_tail());
	check(test_ptrh_int_dbl_remove_from_middle());
	check(test_ptrh_int_dbl_remove_reinsert_trivial());
	check(test_ptrh_int_dbl_remove_reinsert_list());
	return true;
}

#include "test_ptr_hash_types.h"
// PTR_HASH_DEFINE(pint_pdbl, int *, double *);

static bool test_ptrh_ptrs(void)
{
	ptrh_pint_pdbl tbl_, * tbl = &tbl_;
	
	check(ptrh_pint_pdbl_create_sz(tbl, get_ihash, icmp, 4) == tbl);
	
	int foo = 0;
	const int * pfoo = &foo;
	
	double bar = 0.0;
	const double * pbar = &bar;
	
	check(ptrh_pint_pdbl_insert(tbl, &pfoo, &pbar) == tbl);
	check(*ptrh_pint_pdbl_lookup(tbl, &pfoo) == pbar);
	
	ptrh_pint_pdbl_destroy(tbl);
	return true;
}

typedef tbl_node pool_node;

static ptrh_node * ptrht_priv_pool_reach(ptrh_priv_pool * pool, size_t n)
{
	return ptrh_priv_get_elem_addr(pool, n);
}
static size_t ptrht_priv_pool_index(ptrh_priv_pool * pool, const void * elem)
{
	ptrh_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return ptrhv_get_pool_index(tbl, elem);
}
static size_t ptrht_priv_pool_get_next(ptrh_priv_pool * pool, size_t n)
{
	ptrh_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return ptrhv_get_pool_next(tbl, n);
}
static size_t ptrht_priv_pool_head(ptrh_priv_pool * pool)
{
	ptrh_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return ptrhv_pool_head(tbl);
}

#define to_pvoid(x) ((void *)(x))
static bool test_ptrh_hash_priv_pool(void)
{
	ptrh_priv_pool pool_, * pool = &pool_;
	
	check(ptrh_priv_pool_create(pool, 2) == pool);
	check(2 == ptrh_priv_pool_curr_cap(pool));
	check(0 == ptrht_priv_pool_head(pool));
	
	pool_node * node = NULL;
	
	node = (pool_node *)ptrht_priv_pool_reach(pool, 0);
	check(1 == ptrht_priv_pool_index(pool, node->next));
	check(NO_NEXT_N == ptrht_priv_pool_get_next(pool, 1));
	
	node = (pool_node *)ptrht_priv_pool_reach(pool, 1);
	check(NULL == node->next);
	check(NO_NEXT_N == ptrht_priv_pool_index(pool, node->next));
	
	void * elem_a = NULL;
	size_t elem_a_ind = NO_NEXT_N;
	elem_a = ptrh_priv_pool_get(pool);
	elem_a_ind = ptrht_priv_pool_index(pool, elem_a);
	check(0 == elem_a_ind);
	check(1 == ptrht_priv_pool_head(pool));
	ptrht_priv_pool_reach(pool, elem_a_ind)->key = to_pvoid(100);
	check(to_pvoid(100) == ptrht_priv_pool_reach(pool, elem_a_ind)->key);
	
	void * elem_b = NULL;
	size_t elem_b_ind = NO_NEXT_N;
	elem_b = ptrh_priv_pool_get(pool);
	elem_b_ind = ptrht_priv_pool_index(pool, elem_b);
	check(1 == elem_b_ind);
	check(NO_NEXT_N == ptrht_priv_pool_head(pool));
	ptrht_priv_pool_reach(pool, elem_b_ind)->key = to_pvoid(200);
	check(to_pvoid(200) == ptrht_priv_pool_reach(pool, elem_b_ind)->key);
	
	void * elem_c = ptrh_priv_pool_get(pool);
	size_t elem_c_ind = ptrht_priv_pool_index(pool, elem_c);
	check(0 == elem_c_ind);
	check(2 == ptrh_priv_pool_curr_cap(pool));
	check(1 == ptrht_priv_pool_head(pool));
	ptrht_priv_pool_reach(pool, elem_c_ind)->key = to_pvoid(300);
	check(to_pvoid(300) == ptrht_priv_pool_reach(pool, elem_c_ind)->key);
	
	// 0 is the first element of the second slab, cause the second slab is
	// pushed at the front of the pool list
	// 1 is the second element of the second slab;
	// undefined, since nothing has been written there
	// 2 is the first element of the first slab
	// 3 is the second element of the first slab
	
	check(to_pvoid(300) == ptrht_priv_pool_reach(pool, 0)->key);
	check(to_pvoid(100) == ptrht_priv_pool_reach(pool, 2)->key);
	check(to_pvoid(200) == ptrht_priv_pool_reach(pool, 3)->key);
	
	ptrh_priv_pool_destroy(pool);
	return true;
}

static ftest tests[] = {
	test_ptrh_void_create,
	test_ptrh_void_insert,
	test_ptrh_void_iterate,
	test_ptrh_void_rewrite,
	test_ptrh_void_lookup,
	test_ptrh_void_clear,
	test_ptrh_void_copy,
	test_ptrh_void_remove,
	test_ptrh_int_dbl_create,
	test_ptrh_int_dbl_insert,
	test_ptrh_int_dbl_iterate,
	test_ptrh_int_dbl_rewrite,
	test_ptrh_int_dbl_lookup,
	test_ptrh_int_dbl_clear,
	test_ptrh_int_dbl_copy,
	test_ptrh_int_dbl_remove,
	test_ptrh_ptrs,
	test_ptrh_hash_priv_pool,
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
