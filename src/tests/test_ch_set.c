#include <stdio.h>
#include <string.h>
#include "test.h"

#define NO_NEXT   NULL
#define NO_NEXT_N ((size_t)~0)
#define NOT_INIT  chs_priv_not_init_val()
#define UNUSED(x) (void)(x)

#include "ch_set/ch_set.h"

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

static int chhv_get_pool_val(chs_void * chhv, size_t n)
{
	const void * elem = chs_priv_get_elem_addr(&chhv->chain_pool, n);
	return *((int *)elem);
}
static size_t chhv_get_pool_index(chs_void * chhv, const void * elem)
{
	if (!elem)
		goto done;
		
	chs_priv_pool * pool = &chhv->chain_pool;

	void * p = NULL;
	size_t count = 0;
	while ((p = chs_priv_get_elem_addr(pool, count)))
	{
		if (elem == p)
			return count;
		++count;
	}
	
done:
	return NO_NEXT_N;
}
static size_t chhv_get_pool_next(chs_void * chhv, size_t n)
{
	chs_priv_pool * pool = &chhv->chain_pool;
	return chhv_get_pool_index(chhv,
		chs_priv_pool_next_get(pool,
			chs_priv_get_elem_addr(pool, n)));
}
static void * chhv_get_tbl(chs_void * chhv)
{
	return chhv->data;
}
static size_t chhv_pool_head(chs_void * chhv)
{
	return chhv_get_pool_index(chhv,
		chs_priv_pool_get_head(&chhv->chain_pool));
}
static chs_compar chhv_get_cmp(chs_void * chhv)
{
	return chhv->cmp;
}
static chs_get_hash chhv_get_hash(chs_void * chhv)
{
	return chhv->hash;
}

typedef struct tbl_node {
	int key;
	void * next;
} tbl_node;
static bool chhv_is_tbl_clear(chs_void * chhv)
{
	tbl_node * tbl = chhv_get_tbl(chhv);
	for (size_t i = 0, end = chs_void_cap(chhv); i < end; ++i)
	{
		if (chs_priv_not_init_val() != tbl[i].next)
			return false;
	}
	return true;
}
tbl_node chhv_get_tbl_node_at(chs_void * chhv, size_t ind)
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

static bool chhv_test_copy(chs_void * chhv)
{
	node_arr arr_tbl;
	memset(&arr_tbl, 0, sizeof(arr_tbl));
	node_arr arr_cpy;
	memset(&arr_cpy, 0, sizeof(arr_cpy));
	
	chs_void cpy_, * cpy = &cpy_;
	bool is_ok = false;
	*cpy = chs_void_copy(chhv, &is_ok);
	check(is_ok);
	
	check(chs_void_elems(chhv) == chs_void_elems(cpy));
	
	check(chs_priv_pool_curr_cap(&chhv->chain_pool)
		== chs_priv_pool_curr_cap(&cpy->chain_pool));
	
	chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
	chs_iterator cpy_iter_, * cpy_iter = &cpy_iter_;
	
	chs_iterator_init(chhv_iter);
	chs_iterator_init(cpy_iter);
	const void * cvh_key = NULL;
	const void * cpy_key = NULL;
	
	size_t count = 0;
	while ((cvh_key = chs_void_iterate(chhv, chhv_iter)))
	{
		cpy_key = chs_void_iterate(cpy, cpy_iter);
		
		arr_tbl.nodes[count].key = *(int *)cvh_key;
		arr_cpy.nodes[count].key = *(int *)cpy_key;
		++count;
	}
	
	arr_tbl.len = arr_cpy.len = count;
	check(chs_void_elems(chhv) == count);
	
	check(node_arr_eq(&arr_tbl, &arr_cpy));
	
	cpy_key = chs_void_iterate(cpy, cpy_iter);
	check(NULL == cvh_key);
	check(NULL == cpy_key);
	check(cvh_key == cpy_key);
	
	chs_void_destroy(cpy);
	return true;
}

static bool test_chs_void_create(void)
{
	zero_icmp_calls();
	zero_hash_calls();
	
	{
		chs_void cvh_, * cvh = &cvh_;
		check(chs_void_create_sz_pl(cvh, sizeof(int), get_ihash, icmp, 3, 5)
			== cvh);
		
		check(5 == chs_priv_pool_curr_cap(&cvh->chain_pool));
		check(0 == cvh->elems);
		check(chs_void_elems(cvh) == cvh->elems);
		check(chs_void_is_empty(cvh));
		check(chs_void_cap(cvh) == 4);
		check(sizeof(int) == cvh->key_size);
		
		check(chhv_is_tbl_clear(cvh));
		check(chhv_get_tbl(cvh));
		check(chhv_get_cmp(cvh) == icmp);
		check(chhv_get_hash(cvh) == get_ihash);
		
		check(chhv_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		chs_void_destroy(cvh);
	}
	
	{
		chs_void cvh_, * cvh = &cvh_;
		check(chs_void_create(cvh, sizeof(int), get_ihash, icmp) == cvh);
		
		check(CH_SET_DEFAULT_POOL == chs_priv_pool_curr_cap(&cvh->chain_pool));
		check(0 == cvh->elems);
		check(chs_void_elems(cvh) == cvh->elems);
		check(chs_void_is_empty(cvh));
		check(chs_void_cap(cvh) == CH_SET_DEFAULT_SIZE);
		check(sizeof(int) == cvh->key_size);
			
		check(chhv_is_tbl_clear(cvh));
		check(chhv_get_tbl(cvh));
		check(chhv_get_cmp(cvh) == icmp);
		check(chhv_get_hash(cvh) == get_ihash);
		
		check(chhv_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		chs_void_destroy(cvh);
	}
	
	return true;
}

#define ins_val_make(x) ((double)((x)*10+2.43)) // some specific bit pattern

static bool test_chs_void_insert(void)
{
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh, sizeof(int), get_ihash, icmp, 4) == cvh);
	
	int key = 1;
	
	check(chs_void_insert(cvh, &key) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(chs_void_elems(cvh) == 1);
	check(!chs_void_is_empty(cvh));
	
	check(icmp_calls(0));
	check(hash_calls(1));
	
	tbl_node node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(NO_NEXT == node.next);
	
	key = 5;
	check(chs_void_insert(cvh, &key) == cvh);
	// 0 = x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(chs_void_elems(cvh) == 2);
	
	check(icmp_calls(1));
	check(hash_calls(2));
	
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(0 == chhv_get_pool_index(cvh, node.next));
	
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	key = 4;
	check(chs_void_insert(cvh, &key) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(chs_void_elems(cvh) == 3);
	
	check(icmp_calls(1));
	check(hash_calls(3));
	
	node = chhv_get_tbl_node_at(cvh, 0);
	check(4 == node.key);
	check(NO_NEXT == node.next);
	
	key = 7;
	check(chs_void_insert(cvh, &key) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = 7 -> x
	check(chs_void_elems(cvh) == 4);
	
	check(icmp_calls(1));
	check(hash_calls(4));
	
	node = chhv_get_tbl_node_at(cvh, 3);
	check(7 == node.key);
	check(NO_NEXT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	
	// growth event
	key = 12;
	check(chs_void_cap(cvh) == 4);
	check(chs_void_insert(cvh, &key) == cvh);
	check(chs_void_cap(cvh) == 8);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(chs_void_elems(cvh) == 5);
	
	check(icmp_calls(2));
	check(hash_calls(9));
	
	key = 20;
	check(chs_void_insert(cvh, &key) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 20 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(chs_void_elems(cvh) == 6);
	
	check(icmp_calls(4));
	check(hash_calls(10));
	
	node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(NO_NEXT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 4);
	check(4 == node.key);
	check(1 == chhv_get_pool_index(cvh, node.next));
	
	check(chhv_get_pool_val(cvh, 1) == 20);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 12);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	node = chhv_get_tbl_node_at(cvh, 5);
	check(5 == node.key);
	check(NO_NEXT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 6);
	check(NOT_INIT == node.next);
	
	node = chhv_get_tbl_node_at(cvh, 7);
	check(7 == node.key);
	check(NO_NEXT == node.next);
	
	chs_void_destroy(cvh);
	
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	return true;
}

static chs_void * chhv_ins(chs_void * cvh, int * arr, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		chs_void_insert(cvh, arr+i);
	}
	return cvh;
}

static bool chhv_get4(chs_void * cvh)
{
	int arr[] = {1, 5, 4, 7};
	chhv_ins(cvh, arr, 4);
	check(4 == chs_void_elems(cvh));
	check(!chs_void_is_empty(cvh));
	return true;
}

static bool chhv_get6(chs_void * cvh)
{
	int arr[] = {12, 20};
	chhv_get4(cvh);
	chhv_ins(cvh, arr, 2);
	check(6 == chs_void_elems(cvh));
	check(!chs_void_is_empty(cvh));
	return true;
}

static bool test_chs_void_iterate(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh, sizeof(int), get_ihash, icmp, 4) == cvh);
		
	chhv_get4(cvh);
	
	chs_iterator iter_, * iter = &iter_;
	chs_iterator_init(iter);
	
	const void * key = NULL;
	void * val = NULL;
	
	key = chs_void_iterate(cvh, iter);
	check(4 == *(int *)key);
	key = chs_void_iterate(cvh, iter);
	check(1 == *(int *)key);
	key = chs_void_iterate(cvh, iter);
	check(5 == *(int *)key);
	key = chs_void_iterate(cvh, iter);
	check(7 == *(int *)key);
	
	key = chs_void_iterate(cvh, iter);
	check(NULL == key);
	check(NULL == val);
	
	chhv_get6(cvh);
	
	chs_iterator_init(iter);
	key = chs_void_iterate(cvh, iter);
	check(1 == *(int *)key);
	key = chs_void_iterate(cvh, iter);
	check(4 == *(int *)key);
	key = chs_void_iterate(cvh, iter);
	check(20 == *(int *)key);
	key = chs_void_iterate(cvh, iter);
	check(12 == *(int *)key);
	key = chs_void_iterate(cvh, iter);
	check(5 == *(int *)key);
	key = chs_void_iterate(cvh, iter);
	check(7 == *(int *)key);
	
	key = chs_void_iterate(cvh, iter);
	check(NULL == key);
	check(NULL == val);
	
	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_rewrite(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh, sizeof(int), get_ihash, icmp, 4) == cvh);
		
	chhv_get4(cvh);
	
	int key = 1;
	check(chs_void_insert(cvh, &key) == cvh);
	
	key = 4;
	check(chs_void_insert(cvh, &key) == cvh);
	
	key = 5;
	check(chs_void_insert(cvh, &key) == cvh);
		
	chs_iterator iter_, * iter = &iter_;
	chs_iterator_init(iter);
	
	const void * pkey = NULL;
	void * pval = NULL;
	
	pkey = chs_void_iterate(cvh, iter);
	check(4 == *(int *)pkey);
	pkey = chs_void_iterate(cvh, iter);
	check(1 == *(int *)pkey);
	pkey = chs_void_iterate(cvh, iter);
	check(5 == *(int *)pkey);
	pkey = chs_void_iterate(cvh, iter);
	check(7 == *(int *)pkey);
	
	pkey = chs_void_iterate(cvh, iter);
	check(NULL == pkey);
	check(NULL == pval);
		
	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_lookup(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 4) == cvh);
	
	int key = 4;
	check(NULL == chs_void_lookup(cvh, &key));
	
	key = 0;
	check(NULL == chs_void_lookup(cvh, &key));
	
	chhv_get4(cvh);
	key = 4;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 1;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 1000;
	check(NULL == chs_void_lookup(cvh, &key));
	
	key = 5;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 7;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = -1000;
	check(NULL == chs_void_lookup(cvh, &key));
	
	chhv_get6(cvh);
	
	key = 5;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 20;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_clear(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 4) == cvh);
		
	chhv_get4(cvh);
	
	int key = 4;
	
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 1;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 5;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 7;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	size_t h_calls = get_hash_calls();
	size_t cmp_calls = get_icmp_calls();
	check(chs_void_clear(cvh) == cvh);
	check(0 == chs_void_elems(cvh));
	check(chs_void_is_empty(cvh));
	hash_calls(h_calls);
	icmp_calls(cmp_calls);
	
	key = 4;
	check(chs_void_lookup(cvh, &key) == NULL);
	
	key = 1;
	check(chs_void_lookup(cvh, &key) == NULL);
	
	key = 5;
	check(chs_void_lookup(cvh, &key) == NULL);
	
	key = 7;
	check(chs_void_lookup(cvh, &key) == NULL);
	
	chhv_get6(cvh);
	
	key = 1;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 5;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 7;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 4;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 20;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	key = 12;
	check(*(int *)chs_void_lookup(cvh, &key) == key);
	
	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_copy(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 4) == cvh);
	
	check(chhv_test_copy(cvh));
	
	chhv_get4(cvh);
	check(chhv_test_copy(cvh));
	
	chhv_get6(cvh);
	check(chhv_test_copy(cvh));
	
	chs_void_destroy(cvh);
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 5) == cvh);
	
	chhv_get6(cvh);
	check(chhv_test_copy(cvh));
	
	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_remove_from_front(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 4) == cvh);

	int arr[] = {1, 5, 9};
	chhv_ins(cvh, arr, 3);
	check(3 == chs_void_elems(cvh));
	check(!chs_void_is_empty(cvh));
	check(4 == chs_void_cap(cvh));
	
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
	check(1 == chhv_get_pool_index(cvh, node.next));
	
	check(chhv_get_pool_val(cvh, 1) == 9);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(chs_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(chs_void_lookup(cvh, &key));
	key = 5;
	check(chs_void_lookup(cvh, &key));
	key = 9;
	check(chs_void_lookup(cvh, &key));
	check(3 == chs_void_elems(cvh));
	
	key = 1;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(chs_void_lookup(cvh, &key));
	key = 5;
	check(chs_void_lookup(cvh, &key));
	check(2 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh,0) == NO_NEXT_N);
		
		check(1 == chhv_pool_head(cvh));
	}
	
	key = 9;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(chs_void_lookup(cvh, &key));
	check(1 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(NO_NEXT == node.next);
		
		check(0 == chhv_pool_head(cvh));
	}

	key = 5;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chs_void_elems(cvh));
	check(chs_void_is_empty(cvh));
	
	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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

	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_remove_from_tail(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 4) == cvh);

	int arr[] = {1, 5, 9};
	chhv_ins(cvh, arr, 3);
	check(3 == chs_void_elems(cvh));
	check(!chs_void_is_empty(cvh));
	check(4 == chs_void_cap(cvh));
	
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
	check(1 == chhv_get_pool_index(cvh, node.next));

	check(chhv_get_pool_val(cvh, 1) == 9);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(chs_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(chs_void_lookup(cvh, &key));
	key = 5;
	check(chs_void_lookup(cvh, &key));
	key = 9;
	check(chs_void_lookup(cvh, &key));
	check(3 == chs_void_elems(cvh));
	
	key = 5;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 9 -> x
	
	key = 1;
	check(chs_void_lookup(cvh, &key));
	key = 9;
	check(chs_void_lookup(cvh, &key));
	check(2 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(1 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 1) == 9);
		check(chhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == chhv_pool_head(cvh));
	}
	
	key = 9;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> x
	
	key = 1;
	check(chs_void_lookup(cvh, &key));
	check(1 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(NO_NEXT == node.next);
		
		check(1 == chhv_pool_head(cvh));
	}

	key = 1;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chs_void_elems(cvh));
	check(chs_void_is_empty(cvh));
	
	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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

	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_remove_from_middle(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 4) == cvh);

	int arr[] = {1, 5, 9};
	chhv_ins(cvh, arr, 3);
	check(3 == chs_void_elems(cvh));
	check(!chs_void_is_empty(cvh));
	check(4 == chs_void_cap(cvh));
	
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
	check(1 == chhv_get_pool_index(cvh, node.next));

	check(chhv_get_pool_val(cvh, 1) == 9);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key = 10;
	check(chs_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(chs_void_lookup(cvh, &key));
	key = 9;
	check(chs_void_lookup(cvh, &key));
	key = 5;
	check(chs_void_lookup(cvh, &key));
	check(3 == chs_void_elems(cvh));
	
	key = 9;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(chs_void_lookup(cvh, &key));
	key = 5;
	check(chs_void_lookup(cvh, &key));
	check(2 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == chhv_pool_head(cvh));
	}
	
	key = 1;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(chs_void_lookup(cvh, &key));
	check(1 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(NO_NEXT == node.next);
		
		check(0 == chhv_pool_head(cvh));
	}

	key = 5;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chs_void_elems(cvh));
	check(chs_void_is_empty(cvh));
	
	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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

	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_remove_reinsert_list(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 4) == cvh);

	int arr[] = {1, 5, 9};
	chhv_ins(cvh, arr, 3);
	check(3 == chs_void_elems(cvh));
	check(!chs_void_is_empty(cvh));
	check(4 == chs_void_cap(cvh));
	
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
	check(1 == chhv_get_pool_index(cvh, node.next));

	check(chhv_get_pool_val(cvh, 1) == 9);
	check(chhv_get_pool_next(cvh, 1) == 0);
	check(chhv_get_pool_val(cvh, 0) == 5);
	check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(chs_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(chs_void_lookup(cvh, &key));
	key = 5;
	check(chs_void_lookup(cvh, &key));
	key = 9;
	check(chs_void_lookup(cvh, &key));
	check(3 == chs_void_elems(cvh));
	
	key = 9;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(chs_void_lookup(cvh, &key));
	key = 5;
	check(chs_void_lookup(cvh, &key));
	check(2 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == chhv_pool_head(cvh));
	}
	
	key = 9;
	chs_void_insert(cvh, &key);
	check(3 == chs_void_elems(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	
	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(1 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 1) == 9);
		check(chhv_get_pool_next(cvh, 1) == 0);
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == chhv_pool_head(cvh));
	}
	
	key = 1;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(chs_void_lookup(cvh, &key));
	key = 5;
	check(chs_void_lookup(cvh, &key));
	check(2 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == chhv_pool_head(cvh));
	}
	
	key = 1;
	chs_void_insert(cvh, &key);
	check(3 == chs_void_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 1 -> 5 -> x
	
	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(1 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 1) == 1);
		check(chhv_get_pool_next(cvh, 1) == 0);
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == chhv_pool_head(cvh));
	}
	
	// repeat; remove from tail
	
	key = 5;
	check(chs_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!chs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 1 -> x
	
	key = 9;
	check(chs_void_lookup(cvh, &key));
	key = 1;
	check(chs_void_lookup(cvh, &key));
	check(2 == chs_void_elems(cvh));

	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(1 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 1) == 1);
		check(chhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == chhv_pool_head(cvh));
	}
	
	key = 5;
	chs_void_insert(cvh, &key);
	check(3 == chs_void_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 5 -> 1 -> x
	
	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
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
		check(0 == chhv_get_pool_index(cvh, node.next));
		check(chhv_get_pool_val(cvh, 0) == 5);
		check(chhv_get_pool_next(cvh, 0) == 1);
		check(chhv_get_pool_val(cvh, 1) == 1);
		check(chhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(2 == chhv_pool_head(cvh));
	}
	
	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_remove_reinsert_trivial(void)
{
	chs_void cvh_, * cvh = &cvh_;
	
	check(chs_void_create_sz(cvh,
		sizeof(int), get_ihash, icmp, 4) == cvh);
		
	tbl_node node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	int key = 1;
	
	check(chs_void_insert(cvh, &key) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(chs_void_elems(cvh) == 1);
	check(!chs_void_is_empty(cvh));
	
	
	{
		chs_iterator chhv_iter_, * chhv_iter = &chhv_iter_;
		chs_iterator_init(chhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = chs_void_iterate(cvh, chhv_iter);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
	}
	
	node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 1);
	check(1 == node.key);
	check(NO_NEXT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	check(chs_void_remove(cvh, &key) == cvh);
	check(chs_void_is_empty(cvh));
	
	node = chhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = chhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	chs_void_destroy(cvh);
	return true;
}

static bool test_chs_void_remove(void)
{
	check(test_chs_void_remove_from_front());
	check(test_chs_void_remove_from_tail());
	check(test_chs_void_remove_from_middle());
	check(test_chs_void_remove_reinsert_trivial());
	check(test_chs_void_remove_reinsert_list());
	return true;
}

CH_SET_DEFINE(int, int);

static int chhid_get_pool_val(chs_int * chhid, size_t n)
{
	return chhv_get_pool_val(&chhid->tbl, n);
}
static size_t chhid_get_pool_next(chs_int * chhid, size_t n)
{
	return chhv_get_pool_next(&chhid->tbl, n);
}
static size_t chhid_get_pool_index(chs_int * chhid, const void * elem)
{
	return chhv_get_pool_index(&chhid->tbl, elem);
}
static void * chhid_get_tbl(chs_int * chhid)
{
	return chhv_get_tbl(&chhid->tbl);
}
static size_t chhid_pool_head(chs_int * chhid)
{
	return chhv_pool_head(&chhid->tbl);
}
static chs_compar chhid_get_cmp(chs_int * chhid)
{
	return chhv_get_cmp(&chhid->tbl);
}
static chs_get_hash chhid_get_hash(chs_int * chhid)
{
	return chhv_get_hash(&chhid->tbl);
}

static bool chhid_is_tbl_clear(chs_int * chhid)
{
	return chhv_is_tbl_clear(&chhid->tbl);
}
tbl_node chhid_get_tbl_node_at(chs_int * chhid, size_t ind)
{
	return chhv_get_tbl_node_at(&chhid->tbl, ind);
}
static bool chhid_test_copy(chs_int * chhid)
{
	check(chhv_test_copy(&chhid->tbl));
	return true;
}

static bool test_chs_int_create(void)
{
	zero_icmp_calls();
	zero_hash_calls();
	
	{
		chs_int cidh_, * cidh = &cidh_;
		check(chs_int_create_sz_pl(cidh, get_ihash, icmp, 3, 5) == cidh);
		check(5 == chs_priv_pool_curr_cap(&cidh->tbl.chain_pool));
		check(0 == cidh->tbl.elems);
		check(chs_int_elems(cidh) == cidh->tbl.elems);
		check(chs_int_is_empty(cidh));
		check(chs_int_cap(cidh) == 4);
		check(sizeof(int) == cidh->tbl.key_size);
		
		check(chhid_is_tbl_clear(cidh));
		check(chhid_get_tbl(cidh));
		check(chhid_get_cmp(cidh) == icmp);
		check(chhid_get_hash(cidh) == get_ihash);
		
		check(chhid_test_copy(cidh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		chs_int_destroy(cidh);
	}
	
	{
		chs_int cidh_, * cidh = &cidh_;
		check(chs_int_create(cidh, get_ihash, icmp) == cidh);
		check(CH_SET_DEFAULT_POOL
			== chs_priv_pool_curr_cap(&cidh->tbl.chain_pool));
		check(0 == cidh->tbl.elems);
		check(chs_int_elems(cidh) == cidh->tbl.elems);
		check(chs_int_is_empty(cidh));
		check(chs_int_cap(cidh) == CH_SET_DEFAULT_SIZE);
		check(sizeof(int) == cidh->tbl.key_size);
			
		check(chhid_is_tbl_clear(cidh));
		check(chhid_get_tbl(cidh));
		check(chhid_get_cmp(cidh) == icmp);
		check(chhid_get_hash(cidh) == get_ihash);
		
		check(chhid_test_copy(cidh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		chs_int_destroy(cidh);
	}
	
	return true;
}

#define ins_val_make(x) ((double)((x)*10+2.43)) // some specific bit pattern

static bool test_chs_int_insert(void)
{
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);
	
	int key = 1;
	
	check(chs_int_insert(cidh, &key) == cidh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(chs_int_elems(cidh) == 1);
	check(!chs_int_is_empty(cidh));
	
	check(icmp_calls(0));
	check(hash_calls(1));
	
	tbl_node node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(NO_NEXT == node.next);
	
	key = 5;
	check(chs_int_insert(cidh, &key) == cidh);
	// 0 = x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(chs_int_elems(cidh) == 2);
	
	check(icmp_calls(1));
	check(hash_calls(2));
	
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(0 == chhid_get_pool_index(cidh, node.next));
	
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
	
	key = 4;
	check(chs_int_insert(cidh, &key) == cidh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(chs_int_elems(cidh) == 3);
	
	check(icmp_calls(1));
	check(hash_calls(3));
	
	node = chhid_get_tbl_node_at(cidh, 0);
	check(4 == node.key);
	check(NO_NEXT == node.next);
	
	key = 7;
	check(chs_int_insert(cidh, &key) == cidh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = 7 -> x
	check(chs_int_elems(cidh) == 4);
	
	check(icmp_calls(1));
	check(hash_calls(4));
	
	node = chhid_get_tbl_node_at(cidh, 3);
	check(7 == node.key);
	check(NO_NEXT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	
	// growth event
	key = 12;
	check(chs_int_cap(cidh) == 4);
	check(chs_int_insert(cidh, &key) == cidh);
	check(chs_int_cap(cidh) == 8);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(chs_int_elems(cidh) == 5);
	
	check(icmp_calls(2));
	check(hash_calls(9));
	
	key = 20;
	check(chs_int_insert(cidh, &key) == cidh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 20 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(chs_int_elems(cidh) == 6);
	
	check(icmp_calls(4));
	check(hash_calls(10));
	
	node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(NO_NEXT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 4);
	check(4 == node.key);
	check(1 == chhid_get_pool_index(cidh, node.next));
	
	check(chhid_get_pool_val(cidh, 1) == 20);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 12);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
	
	node = chhid_get_tbl_node_at(cidh, 5);
	check(5 == node.key);
	check(NO_NEXT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 6);
	check(NOT_INIT == node.next);
	
	node = chhid_get_tbl_node_at(cidh, 7);
	check(7 == node.key);
	check(NO_NEXT == node.next);
	
	chs_int_destroy(cidh);
	
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	return true;
}

static chs_int * chhid_ins(chs_int * cidh, int * arr, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		chs_int_insert(cidh, arr+i);
	}
	return cidh;
}

static bool chhid_get4(chs_int * cidh)
{
	int arr[] = {1, 5, 4, 7};
	chhid_ins(cidh, arr, 4);
	check(4 == chs_int_elems(cidh));
	check(!chs_int_is_empty(cidh));
	return true;
}

static bool chhid_get6(chs_int * cidh)
{
	int arr[] = {12, 20};
	chhid_get4(cidh);
	chhid_ins(cidh, arr, 2);
	check(6 == chs_int_elems(cidh));
	check(!chs_int_is_empty(cidh));
	return true;
}

static bool test_chs_int_iterate(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);
		
	chhid_get4(cidh);
	
	chs_iterator iter_, * iter = &iter_;
	chs_iterator_init(iter);
	
	const int * key = NULL;
	
	key = chs_int_iterate(cidh, iter);
	check(4 == *key);
	key = chs_int_iterate(cidh, iter);
	check(1 == *key);
	key = chs_int_iterate(cidh, iter);
	check(5 == *key);
	key = chs_int_iterate(cidh, iter);
	check(7 == *key);
	
	key = chs_int_iterate(cidh, iter);
	check(NULL == key);
	
	chhid_get6(cidh);
	
	chs_iterator_init(iter);
	key = chs_int_iterate(cidh, iter);
	check(1 == *key);
	key = chs_int_iterate(cidh, iter);
	check(4 == *key);
	key = chs_int_iterate(cidh, iter);
	check(20 == *key);
	key = chs_int_iterate(cidh, iter);
	check(12 == *key);
	key = chs_int_iterate(cidh, iter);
	check(5 == *key);
	key = chs_int_iterate(cidh, iter);
	check(7 == *key);
	
	key = chs_int_iterate(cidh, iter);
	check(NULL == key);
	
	chs_int_destroy(cidh);
	return true;
}

static bool test_chs_int_rewrite(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);
		
	chhid_get4(cidh);
	
	int key = 1;
	check(chs_int_insert(cidh, &key) == cidh);
	
	key = 4;
	check(chs_int_insert(cidh, &key) == cidh);
	
	key = 5;
	check(chs_int_insert(cidh, &key) == cidh);
		
	chs_iterator iter_, * iter = &iter_;
	chs_iterator_init(iter);
	
	const int * pkey = NULL;
	double * pval = NULL;
	
	pkey = chs_int_iterate(cidh, iter);
	check(4 == *pkey);
	pkey = chs_int_iterate(cidh, iter);
	check(1 == *pkey);
	pkey = chs_int_iterate(cidh, iter);
	check(5 == *pkey);
	pkey = chs_int_iterate(cidh, iter);
	check(7 == *pkey);
	
	pkey = chs_int_iterate(cidh, iter);
	check(NULL == pkey);
	check(NULL == pval);
		
	chs_int_destroy(cidh);
	return true;
}


static bool test_chs_int_lookup(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);
	
	int key = 4;
	check(NULL == chs_int_lookup(cidh, &key));
	
	key = 0;
	check(NULL == chs_int_lookup(cidh, &key));
	
	chhid_get4(cidh);
	key = 4;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 1;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 1000;
	check(NULL == chs_int_lookup(cidh, &key));
	
	key = 5;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 7;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = -1000;
	check(NULL == chs_int_lookup(cidh, &key));
	
	chhid_get6(cidh);
	
	key = 5;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 20;
	check(*chs_int_lookup(cidh, &key) == key);
	
	chs_int_destroy(cidh);
	return true;
}

static bool test_chs_int_clear(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);
		
	chhid_get4(cidh);
	
	int key = 4;
	
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 1;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 5;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 7;
	check(*chs_int_lookup(cidh, &key) == key);
	
	size_t h_calls = get_hash_calls();
	size_t cmp_calls = get_icmp_calls();
	check(chs_int_clear(cidh) == cidh);
	check(0 == chs_int_elems(cidh));
	check(chs_int_is_empty(cidh));
	hash_calls(h_calls);
	icmp_calls(cmp_calls);
	
	key = 4;
	check(chs_int_lookup(cidh, &key) == NULL);
	
	key = 1;
	check(chs_int_lookup(cidh, &key) == NULL);
	
	key = 5;
	check(chs_int_lookup(cidh, &key) == NULL);
	
	key = 7;
	check(chs_int_lookup(cidh, &key) == NULL);
	
	chhid_get6(cidh);
	
	key = 1;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 5;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 7;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 4;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 20;
	check(*chs_int_lookup(cidh, &key) == key);
	
	key = 12;
	check(*chs_int_lookup(cidh, &key) == key);
	
	chs_int_destroy(cidh);
	return true;
}

static bool test_chs_int_copy(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);
	
	check(chhid_test_copy(cidh));
	
	chhid_get4(cidh);
	check(chhid_test_copy(cidh));
	
	chhid_get6(cidh);
	check(chhid_test_copy(cidh));
	
	chs_int_destroy(cidh);
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 5) == cidh);
	
	chhid_get6(cidh);
	check(chhid_test_copy(cidh));
	
	chs_int_destroy(cidh);
	return true;
}

static bool test_chs_int_remove_from_front(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);

	int arr[] = {1, 5, 9};
	chhid_ins(cidh, arr, 3);
	check(3 == chs_int_elems(cidh));
	check(!chs_int_is_empty(cidh));
	check(4 == chs_int_cap(cidh));
	
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
	check(1 == chhid_get_pool_index(cidh, node.next));
	
	check(chhid_get_pool_val(cidh, 1) == 9);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);

	int key = 10;
	check(chs_int_remove(cidh, &key) == cidh);
	
	key = 1;
	check(chs_int_lookup(cidh, &key));
	key = 5;
	check(chs_int_lookup(cidh, &key));
	key = 9;
	check(chs_int_lookup(cidh, &key));
	check(3 == chs_int_elems(cidh));
	
	key = 1;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 1;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(chs_int_lookup(cidh, &key));
	key = 5;
	check(chs_int_lookup(cidh, &key));
	check(2 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 9);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh,0) == NO_NEXT_N);
		
		check(1 == chhid_pool_head(cidh));
	}
	
	key = 9;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 9;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(chs_int_lookup(cidh, &key));
	check(1 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(NO_NEXT == node.next);
		
		check(0 == chhid_pool_head(cidh));
	}

	key = 5;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 5;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chs_int_elems(cidh));
	check(chs_int_is_empty(cidh));
	
	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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

	chs_int_destroy(cidh);
	return true;
}

static bool test_chs_int_remove_from_tail(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);

	int arr[] = {1, 5, 9};
	chhid_ins(cidh, arr, 3);
	check(3 == chs_int_elems(cidh));
	check(!chs_int_is_empty(cidh));
	check(4 == chs_int_cap(cidh));
	
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
	check(1 == chhid_get_pool_index(cidh, node.next));

	check(chhid_get_pool_val(cidh, 1) == 9);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);

	int key = 10;
	check(chs_int_remove(cidh, &key) == cidh);
	
	key = 1;
	check(chs_int_lookup(cidh, &key));
	key = 5;
	check(chs_int_lookup(cidh, &key));
	key = 9;
	check(chs_int_lookup(cidh, &key));
	check(3 == chs_int_elems(cidh));
	
	key = 5;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 5;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 1 -> 9 -> x
	
	key = 1;
	check(chs_int_lookup(cidh, &key));
	key = 9;
	check(chs_int_lookup(cidh, &key));
	check(2 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 9);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(1 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 1) == 9);
		check(chhid_get_pool_next(cidh, 1) == NO_NEXT_N);
		
		check(0 == chhid_pool_head(cidh));
	}
	
	key = 9;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 9;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 1 -> x
	
	key = 1;
	check(chs_int_lookup(cidh, &key));
	check(1 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(NO_NEXT == node.next);
		
		check(1 == chhid_pool_head(cidh));
	}

	key = 1;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 1;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chs_int_elems(cidh));
	check(chs_int_is_empty(cidh));
	
	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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

	chs_int_destroy(cidh);
	return true;
}

static bool test_chs_int_remove_from_middle(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);

	int arr[] = {1, 5, 9};
	chhid_ins(cidh, arr, 3);
	check(3 == chs_int_elems(cidh));
	check(!chs_int_is_empty(cidh));
	check(4 == chs_int_cap(cidh));
	
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
	check(1 == chhid_get_pool_index(cidh, node.next));

	check(chhid_get_pool_val(cidh, 1) == 9);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
	
	int key = 10;
	check(chs_int_remove(cidh, &key) == cidh);
	
	key = 1;
	check(chs_int_lookup(cidh, &key));
	key = 9;
	check(chs_int_lookup(cidh, &key));
	key = 5;
	check(chs_int_lookup(cidh, &key));
	check(3 == chs_int_elems(cidh));
	
	key = 9;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 9;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(chs_int_lookup(cidh, &key));
	key = 5;
	check(chs_int_lookup(cidh, &key));
	check(2 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(1 == chhid_pool_head(cidh));
	}
	
	key = 1;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 1;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(chs_int_lookup(cidh, &key));
	check(1 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(NO_NEXT == node.next);
		
		check(0 == chhid_pool_head(cidh));
	}

	key = 5;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 5;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == chs_int_elems(cidh));
	check(chs_int_is_empty(cidh));
	
	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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

	chs_int_destroy(cidh);
	return true;
}


static bool test_chs_int_remove_reinsert_list(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);

	int arr[] = {1, 5, 9};
	chhid_ins(cidh, arr, 3);
	check(3 == chs_int_elems(cidh));
	check(!chs_int_is_empty(cidh));
	check(4 == chs_int_cap(cidh));
	
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
	check(1 == chhid_get_pool_index(cidh, node.next));

	check(chhid_get_pool_val(cidh, 1) == 9);
	check(chhid_get_pool_next(cidh, 1) == 0);
	check(chhid_get_pool_val(cidh, 0) == 5);
	check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);

	int key = 10;
	check(chs_int_remove(cidh, &key) == cidh);
	
	key = 1;
	check(chs_int_lookup(cidh, &key));
	key = 5;
	check(chs_int_lookup(cidh, &key));
	key = 9;
	check(chs_int_lookup(cidh, &key));
	check(3 == chs_int_elems(cidh));
	
	key = 9;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 9;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(chs_int_lookup(cidh, &key));
	key = 5;
	check(chs_int_lookup(cidh, &key));
	check(2 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(1 == chhid_pool_head(cidh));
	}
	
	key = 9;
	chs_int_insert(cidh, &key);
	check(3 == chs_int_elems(cidh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	
	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 9);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(1 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 1) == 9);
		check(chhid_get_pool_next(cidh, 1) == 0);
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(2 == chhid_pool_head(cidh));
	}
	
	key = 1;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 1;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(chs_int_lookup(cidh, &key));
	key = 5;
	check(chs_int_lookup(cidh, &key));
	check(2 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 9);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(1 == chhid_pool_head(cidh));
	}
	
	key = 1;
	chs_int_insert(cidh, &key);
	check(3 == chs_int_elems(cidh));
	
	// 0 = x
	// 1 = 9 -> 1 -> 5 -> x
	
	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 9);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(1 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 1) == 1);
		check(chhid_get_pool_next(cidh, 1) == 0);
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == NO_NEXT_N);
		
		check(2 == chhid_pool_head(cidh));
	}
	
	// repeat; remove from tail
	
	key = 5;
	check(chs_int_remove(cidh, &key) == cidh);
	key = 5;
	check(!chs_int_lookup(cidh, &key));
	
	// 0 = x
	// 1 = 9 -> 1 -> x
	
	key = 9;
	check(chs_int_lookup(cidh, &key));
	key = 1;
	check(chs_int_lookup(cidh, &key));
	check(2 == chs_int_elems(cidh));

	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 9);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(1 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 1) == 1);
		check(chhid_get_pool_next(cidh, 1) == NO_NEXT_N);
		
		check(0 == chhid_pool_head(cidh));
	}
	
	key = 5;
	chs_int_insert(cidh, &key);
	check(3 == chs_int_elems(cidh));
	
	// 0 = x
	// 1 = 9 -> 5 -> 1 -> x
	
	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 9);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 5);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
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
		check(0 == chhid_get_pool_index(cidh, node.next));
		check(chhid_get_pool_val(cidh, 0) == 5);
		check(chhid_get_pool_next(cidh, 0) == 1);
		check(chhid_get_pool_val(cidh, 1) == 1);
		check(chhid_get_pool_next(cidh, 1) == NO_NEXT_N);
		
		check(2 == chhid_pool_head(cidh));
	}
	
	chs_int_destroy(cidh);
	return true;
}

static bool test_chs_int_remove_reinsert_trivial(void)
{
	chs_int cidh_, * cidh = &cidh_;
	
	check(chs_int_create_sz(cidh, get_ihash, icmp, 4) == cidh);
		
	tbl_node node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 1);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	int key = 1;
	
	check(chs_int_insert(cidh, &key) == cidh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(chs_int_elems(cidh) == 1);
	check(!chs_int_is_empty(cidh));
	
	
	{
		chs_iterator chhid_iter_, * chhid_iter = &chhid_iter_;
		chs_iterator_init(chhid_iter);
		
		const int * cidh_key = NULL;
		double * cidh_val = NULL;
		
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check((int)(*cidh_key) == 1);
		cidh_key = chs_int_iterate(cidh, chhid_iter);
		check(NULL == cidh_key);
		check(NULL == cidh_val);
	}
	
	node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 1);
	check(1 == node.key);
	check(NO_NEXT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	check(chs_int_remove(cidh, &key) == cidh);
	check(chs_int_is_empty(cidh));
	
	node = chhid_get_tbl_node_at(cidh, 0);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 1);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 2);
	check(NOT_INIT == node.next);
	node = chhid_get_tbl_node_at(cidh, 3);
	check(NOT_INIT == node.next);
	
	chs_int_destroy(cidh);
	return true;
}

static bool test_chs_int_remove(void)
{
	check(test_chs_int_remove_from_front());
	check(test_chs_int_remove_from_tail());
	check(test_chs_int_remove_from_middle());
	check(test_chs_int_remove_reinsert_trivial());
	check(test_chs_int_remove_reinsert_list());
	return true;
}


CH_SET_DEFINE(pint, int *);

static bool test_chs_ptrs(void)
{
	chs_pint tbl_, * tbl = &tbl_;
	
	check(chs_pint_create_sz(tbl, get_ihash, icmp, 4) == tbl);
	
	int foo = 0;
	const int * pfoo = &foo;
	
	check(chs_pint_insert(tbl, &pfoo) == tbl);
	check(*chs_pint_lookup(tbl, &pfoo) == pfoo);
	
	chs_pint_destroy(tbl);
	return true;
}

typedef struct pool_node {
	size_t n;
	void * next;
} pool_node;


static void * chht_priv_pool_reach(chs_priv_pool * pool, size_t n)
{
	return chs_priv_get_elem_addr(pool, n);
}
static size_t chht_priv_pool_index(chs_priv_pool * pool, const void * elem)
{
	chs_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return chhv_get_pool_index(tbl, elem);
}
static size_t chht_priv_pool_get_next(chs_priv_pool * pool, size_t n)
{
	chs_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return chhv_get_pool_next(tbl, n);
}
static size_t chht_priv_pool_head(chs_priv_pool * pool)
{
	chs_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return chhv_pool_head(tbl);
}

static bool test_ch_set_priv_pool(void)
{
	chs_priv_pool pool_, * pool = &pool_;
	
	check(chs_priv_pool_create(pool, sizeof(size_t), 2) == pool);
	check(chs_priv_pool_elem_size(pool) == sizeof(size_t)*2);
	check(2 == chs_priv_pool_curr_cap(pool));
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
	elem_a = chs_priv_pool_get(pool);
	elem_a_ind = chht_priv_pool_index(pool, elem_a);
	check(0 == elem_a_ind);
	check(1 == chht_priv_pool_head(pool));
	*(size_t *)chht_priv_pool_reach(pool, elem_a_ind) = 100;
	check(100 == *(size_t *)chht_priv_pool_reach(pool, elem_a_ind));
	
	void * elem_b = NULL;
	size_t elem_b_ind = NO_NEXT_N;
	elem_b = chs_priv_pool_get(pool);
	elem_b_ind = chht_priv_pool_index(pool, elem_b);
	check(1 == elem_b_ind);
	check(NO_NEXT_N == chht_priv_pool_head(pool));
	*(size_t *)chht_priv_pool_reach(pool, elem_b_ind) = 200;
	check(200 == *(size_t *)chht_priv_pool_reach(pool, elem_b_ind));
	
	void * elem_c = chs_priv_pool_get(pool);
	size_t elem_c_ind = chht_priv_pool_index(pool, elem_c);
	check(0 == elem_c_ind);
	check(2 == chs_priv_pool_curr_cap(pool));
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
	
	chs_priv_pool_destroy(pool);
	return true;
}

static ftest tests[] = {
	test_chs_void_create,
	test_chs_void_insert,
	test_chs_void_iterate,
	test_chs_void_rewrite,
	test_chs_void_lookup,
	test_chs_void_clear,
	test_chs_void_copy,
	test_chs_void_remove,
	test_chs_int_create,
	test_chs_int_insert,
	test_chs_int_iterate,
	test_chs_int_rewrite,
	test_chs_int_lookup,
	test_chs_int_clear,
	test_chs_int_copy,
	test_chs_int_remove,
	test_chs_ptrs,
	test_ch_set_priv_pool
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
