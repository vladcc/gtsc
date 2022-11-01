#include <stdio.h>
#include <string.h>
#include "test.h"

#include "ptr_set/ptr_set.h"

#define NO_NEXT   NULL
#define NO_NEXT_N ((size_t)~0)
#define NOT_INIT  ptrs_priv_not_init_val()
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

static ptrs_compar ptrhv_get_cmp(ptrs_void * chhv)
{
	return chhv->cmp;
}
static ptrs_get_hash ptrhv_get_hash(ptrs_void * chhv)
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

static int ptrhv_get_pool_val(ptrs_void * chhv, size_t n)
{
	const ptrs_node * elem = ptrs_priv_get_elem_addr(&chhv->chain_pool, n);
	return *((int *)elem->key);
}
static size_t ptrhv_get_pool_index(ptrs_void * chhv, const void * elem)
{
	if (!elem)
		goto done;
		
	ptrs_priv_pool * pool = &chhv->chain_pool;

	ptrs_node * p = NULL;
	size_t count = 0;
	while ((p = ptrs_priv_get_elem_addr(pool, count)))
	{
		if (elem == p)
			return count;
		++count;
	}
	
done:
	return NO_NEXT_N;
}
static size_t ptrhv_get_pool_next(ptrs_void * chhv, size_t n)
{
	ptrs_priv_pool * pool = &chhv->chain_pool;
	return ptrhv_get_pool_index(chhv,
		ptrs_priv_pool_next_get(pool,
			ptrs_priv_get_elem_addr(pool, n)));
}
static size_t ptrhv_pool_head(ptrs_void * chhv)
{
	return ptrhv_get_pool_index(chhv,
		ptrs_priv_pool_get_head(&chhv->chain_pool));
}

typedef ptrs_node tbl_node;
static tbl_node * ptrhv_get_tbl(ptrs_void * chhv)
{
	return chhv->data;
}
static bool ptrhv_is_tbl_clear(ptrs_void * chhv)
{
	tbl_node * tbl = ptrhv_get_tbl(chhv);
	for (size_t i = 0, end = ptrs_void_cap(chhv); i < end; ++i)
	{
		if (ptrs_priv_not_init_val() != tbl[i].next)
			return false;
	}
	return true;
}
tbl_node ptrhv_get_tbl_node_at(ptrs_void * chhv, size_t ind)
{
	tbl_node * tbl = ptrhv_get_tbl(chhv);
	return tbl[ind];
}

typedef struct arr_node {
	int key;
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
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	
	return true;
}
static bool ptrhv_test_copy(ptrs_void * chhv)
{
	node_arr arr_tbl;
	memset(&arr_tbl, 0, sizeof(arr_tbl));
	node_arr arr_cpy;
	memset(&arr_cpy, 0, sizeof(arr_cpy));
	
	ptrs_void cpy_, * cpy = &cpy_;
	bool is_ok = false;
	*cpy = ptrs_void_copy(chhv, &is_ok);
	check(is_ok);
	
	check(ptrs_void_elems(chhv) == ptrs_void_elems(cpy));
	check(ptrs_priv_pool_curr_cap(&chhv->chain_pool)
		== ptrs_priv_pool_curr_cap(&cpy->chain_pool));
	
	ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
	ptrs_iterator cpy_iter_, * cpy_iter = &cpy_iter_;
	
	ptrs_iterator_init(ptrhv_iter);
	ptrs_iterator_init(cpy_iter);
	const void * cvh_key = NULL;
	void * cvh_val = NULL;
	const void * cpy_key = NULL;
	void * cpy_val = NULL;
	
	size_t count = 0;
	while ((cvh_key = ptrs_void_iterate(chhv, ptrhv_iter)))
	{
		cpy_key = ptrs_void_iterate(cpy, cpy_iter);
		
		arr_tbl.nodes[count].key = *(int *)cvh_key;
		
		arr_cpy.nodes[count].key = *(int *)cpy_key;
		
		++count;
	}
	
	arr_tbl.len = arr_cpy.len = count;
	check(ptrs_void_elems(chhv) == count);
	
	check(node_arr_eq(&arr_tbl, &arr_cpy));
	
	cpy_key = ptrs_void_iterate(cpy, cpy_iter);
	check(NULL == cvh_key);
	check(NULL == cpy_key);
	check(cvh_key == cpy_key);
	
	check(NULL == cvh_val);
	check(NULL == cpy_val);
	check(cvh_val == cpy_val);
	
	ptrs_void_destroy(cpy);
	return true;
}

static bool test_ptrs_void_create(void)
{
	zero_icmp_calls();
	zero_hash_calls();
	
	{
		ptrs_void cvh_, * cvh = &cvh_;
		check(ptrs_void_create_sz_pl(cvh, get_ihash, icmp, 3, 5) == cvh);
		
		check(5 == ptrs_priv_pool_curr_cap(&cvh->chain_pool));
		check(0 == cvh->elems);
		check(ptrs_void_elems(cvh) == cvh->elems);
		check(ptrs_void_is_empty(cvh));
		check(ptrs_void_cap(cvh) == 4);
		
		check(ptrhv_is_tbl_clear(cvh));
		check(ptrhv_get_tbl(cvh));
		check(ptrhv_get_cmp(cvh) == icmp);
		check(ptrhv_get_hash(cvh) == get_ihash);
		
		check(ptrhv_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		ptrs_void_destroy(cvh);
	}
	
	{
		ptrs_void cvh_, * cvh = &cvh_;
		check(ptrs_void_create(cvh, get_ihash, icmp) == cvh);
		
		check(PTR_SET_DEFAULT_POOL
			== ptrs_priv_pool_curr_cap(&cvh->chain_pool));
		check(0 == cvh->elems);
		check(ptrs_void_elems(cvh) == cvh->elems);
		check(ptrs_void_is_empty(cvh));
		check(ptrs_void_cap(cvh) == PTR_SET_DEFAULT_SIZE);
			
		check(ptrhv_is_tbl_clear(cvh));
		check(ptrhv_get_tbl(cvh));
		check(ptrhv_get_cmp(cvh) == icmp);
		check(ptrhv_get_hash(cvh) == get_ihash);
		
		check(ptrhv_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		ptrs_void_destroy(cvh);
	}
	
	return true;
}

static bool test_ptrs_void_insert(void)
{
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	int key_1 = 1;
	
	check(ptrs_void_insert(cvh, &key_1) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(ptrs_void_elems(cvh) == 1);
	check(!ptrs_void_is_empty(cvh));
	
	check(icmp_calls(0));
	check(hash_calls(1));
	
	tbl_node node = ptrhv_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	int key_2 = 5;
	check(ptrs_void_insert(cvh, &key_2) == cvh);
	// 0 = x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(ptrs_void_elems(cvh) == 2);
	
	check(icmp_calls(1));
	check(hash_calls(2));
	
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(0 == ptrhv_get_pool_index(cvh, node.next));
	
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key_3 = 4;
	check(ptrs_void_insert(cvh, &key_3) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(ptrs_void_elems(cvh) == 3);
	
	check(icmp_calls(1));
	check(hash_calls(3));
	
	node = ptrhv_get_tbl_node_at(cvh, 0);
	check(&key_3 == node.key);
	check(4 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	int key_4 = 7;
	check(ptrs_void_insert(cvh, &key_4) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = 7 -> x
	check(ptrs_void_elems(cvh) == 4);
	
	check(icmp_calls(1));
	check(hash_calls(4));
	
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(&key_4 == node.key);
	check(7 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	
	// growth event
	int key_5 = 12;
	check(ptrs_void_cap(cvh) == 4);
	check(ptrs_void_insert(cvh, &key_5) == cvh);
	check(ptrs_void_cap(cvh) == 8);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(ptrs_void_elems(cvh) == 5);
	
	check(icmp_calls(2));
	check(hash_calls(9));
	
	int key_6 = 20;
	check(ptrs_void_insert(cvh, &key_6) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 20 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(ptrs_void_elems(cvh) == 6);
	
	check(icmp_calls(4));
	check(hash_calls(10));
	
	node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 4);
	check(&key_3 == node.key);
	check(4 == *(int *)node.key);
	check(1 == ptrhv_get_pool_index(cvh, node.next));
	
	check(ptrhv_get_pool_val(cvh, 1) == 20);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 12);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	node = ptrhv_get_tbl_node_at(cvh, 5);
	check(&key_2 == node.key);
	check(5 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 6);
	check(NOT_INIT == node.next);
	
	node = ptrhv_get_tbl_node_at(cvh, 7);
	check(&key_4 == node.key);
	check(7 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	ptrs_void_destroy(cvh);
	
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	return true;
}

typedef struct data_node {
	int key;
} data_node;
static ptrs_void * ptrhv_ins(ptrs_void * cvh, data_node * arr, size_t len)
{
	for (size_t i = 0; i < len; ++i)
		ptrs_void_insert(cvh, &arr[i].key);
	return cvh;
}
static bool ptrhv_get4(ptrs_void * cvh)
{
	static data_node arr[] = {
		{1},
		{5},
		{4},
		{7},
	};
	
	ptrhv_ins(cvh, arr, 4);
	check(4 == ptrs_void_elems(cvh));
	check(!ptrs_void_is_empty(cvh));
	return true;
}
static bool ptrhv_get6(ptrs_void * cvh)
{
	static data_node arr[] = {
		{12},
		{20},
	};
	
	ptrhv_get4(cvh);
	ptrhv_ins(cvh, arr, 2);
	check(6 == ptrs_void_elems(cvh));
	check(!ptrs_void_is_empty(cvh));
	return true;
}

static bool test_ptrs_void_iterate(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhv_get4(cvh);
	
	ptrs_iterator iter_, * iter = &iter_;
	ptrs_iterator_init(iter);
	
	const void * key = NULL;
	void * val = NULL;
	
	key = ptrs_void_iterate(cvh, iter);
	check(4 == *(int *)key);
	key = ptrs_void_iterate(cvh, iter);
	check(1 == *(int *)key);
	key = ptrs_void_iterate(cvh, iter);
	check(5 == *(int *)key);
	key = ptrs_void_iterate(cvh, iter);
	check(7 == *(int *)key);
	
	key = ptrs_void_iterate(cvh, iter);
	check(NULL == key);
	check(NULL == val);
	
	ptrhv_get6(cvh);
	
	ptrs_iterator_init(iter);
	key = ptrs_void_iterate(cvh, iter);
	check(1 == *(int *)key);
	key = ptrs_void_iterate(cvh, iter);
	check(4 == *(int *)key);
	key = ptrs_void_iterate(cvh, iter);
	check(20 == *(int *)key);
	key = ptrs_void_iterate(cvh, iter);
	check(12 == *(int *)key);
	key = ptrs_void_iterate(cvh, iter);
	check(5 == *(int *)key);
	key = ptrs_void_iterate(cvh, iter);
	check(7 == *(int *)key);
	
	key = ptrs_void_iterate(cvh, iter);
	check(NULL == key);
	check(NULL == val);
	
	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_rewrite(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhv_get4(cvh);
	
	int key_1 = 1;
	check(ptrs_void_insert(cvh, &key_1) == cvh);
	
	int key_2 = 4;
	check(ptrs_void_insert(cvh, &key_2) == cvh);
	
	int key_3 = 5;
	check(ptrs_void_insert(cvh, &key_3) == cvh);
		
	ptrs_iterator iter_, * iter = &iter_;
	ptrs_iterator_init(iter);
	
	const void * pkey = NULL;
	
	pkey = ptrs_void_iterate(cvh, iter);
	check(4 == *(int *)pkey);
	pkey = ptrs_void_iterate(cvh, iter);
	check(1 == *(int *)pkey);
	pkey = ptrs_void_iterate(cvh, iter);
	check(5 == *(int *)pkey);
	pkey = ptrs_void_iterate(cvh, iter);
	check(7 == *(int *)pkey);
	
	pkey = ptrs_void_iterate(cvh, iter);
	check(NULL == pkey);
		
	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_lookup(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	int key = 4;
	const void * pkey = NULL;
	check(NULL == ptrs_void_lookup(cvh, &key));
	
	key = 0;
	check(NULL == ptrs_void_lookup(cvh, &key));
	
	ptrhv_get4(cvh);
	key = 4;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 1;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 1000;
	check(NULL == ptrs_void_lookup(cvh, &key));
	
	key = 5;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 7;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = -1000;
	check(NULL == ptrs_void_lookup(cvh, &key));
	
	ptrhv_get6(cvh);
	
	key = 5;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 20;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_clear(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhv_get4(cvh);
	
	int key = 4;
	const void * pkey = NULL;
	
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 1;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 5;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 7;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	size_t h_calls = get_hash_calls();
	size_t cmp_calls = get_icmp_calls();
	check(ptrs_void_clear(cvh) == cvh);
	check(0 == ptrs_void_elems(cvh));
	check(ptrs_void_is_empty(cvh));
	hash_calls(h_calls);
	icmp_calls(cmp_calls);
	
	key = 4;
	pkey = ptrs_void_lookup(cvh, &key);
	
	key = 1;
	pkey = ptrs_void_lookup(cvh, &key);
	
	key = 5;
	pkey = ptrs_void_lookup(cvh, &key);
	
	key = 7;
	pkey = ptrs_void_lookup(cvh, &key);
	
	ptrhv_get6(cvh);
	
	key = 1;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 5;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 7;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 4;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 20;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	key = 12;
	pkey = ptrs_void_lookup(cvh, &key);
	check(*(int *)pkey == key);
	
	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_copy(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	check(ptrhv_test_copy(cvh));
	
	ptrhv_get4(cvh);
	check(ptrhv_test_copy(cvh));
	
	ptrhv_get6(cvh);
	check(ptrhv_test_copy(cvh));
	
	ptrs_void_destroy(cvh);
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 5) == cvh);
	
	ptrhv_get6(cvh);
	check(ptrhv_test_copy(cvh));
	
	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_remove_from_front(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1},
		{5},
		{9},
	};

	ptrhv_ins(cvh, arr, 3);
	check(3 == ptrs_void_elems(cvh));
	check(!ptrs_void_is_empty(cvh));
	check(4 == ptrs_void_cap(cvh));
	
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
	check(1 == ptrhv_get_pool_index(cvh, node.next));
	
	check(ptrhv_get_pool_val(cvh, 1) == 9);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(ptrs_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrs_void_lookup(cvh, &key));
	key = 5;
	check(ptrs_void_lookup(cvh, &key));
	key = 9;
	check(ptrs_void_lookup(cvh, &key));
	check(3 == ptrs_void_elems(cvh));
	
	key = 1;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(ptrs_void_lookup(cvh, &key));
	key = 5;
	check(ptrs_void_lookup(cvh, &key));
	check(2 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh,0) == NO_NEXT_N);
		
		check(1 == ptrhv_pool_head(cvh));
	}
	
	key = 9;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(ptrs_void_lookup(cvh, &key));
	check(1 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(NO_NEXT == node.next);
		
		check(0 == ptrhv_pool_head(cvh));
	}

	key = 5;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrs_void_elems(cvh));
	check(ptrs_void_is_empty(cvh));
	
	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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

	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_remove_from_tail(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1},
		{5},
		{9},
	};
	
	ptrhv_ins(cvh, arr, 3);
	check(3 == ptrs_void_elems(cvh));
	check(!ptrs_void_is_empty(cvh));
	check(4 == ptrs_void_cap(cvh));
	
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
	check(1 == ptrhv_get_pool_index(cvh, node.next));

	check(ptrhv_get_pool_val(cvh, 1) == 9);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(ptrs_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrs_void_lookup(cvh, &key));
	key = 5;
	check(ptrs_void_lookup(cvh, &key));
	key = 9;
	check(ptrs_void_lookup(cvh, &key));
	check(3 == ptrs_void_elems(cvh));
	
	key = 5;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 9 -> x
	
	key = 1;
	check(ptrs_void_lookup(cvh, &key));
	key = 9;
	check(ptrs_void_lookup(cvh, &key));
	check(2 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(1 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 1) == 9);
		check(ptrhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == ptrhv_pool_head(cvh));
	}
	
	key = 9;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> x
	
	key = 1;
	check(ptrs_void_lookup(cvh, &key));
	check(1 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(NO_NEXT == node.next);
		
		check(1 == ptrhv_pool_head(cvh));
	}

	key = 1;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrs_void_elems(cvh));
	check(ptrs_void_is_empty(cvh));
	
	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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

	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_remove_from_middle(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1},
		{5},
		{9},
	};
	
	ptrhv_ins(cvh, arr, 3);
	check(3 == ptrs_void_elems(cvh));
	check(!ptrs_void_is_empty(cvh));
	check(4 == ptrs_void_cap(cvh));
	
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
	check(1 == ptrhv_get_pool_index(cvh, node.next));

	check(ptrhv_get_pool_val(cvh, 1) == 9);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key = 10;
	check(ptrs_void_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrs_void_lookup(cvh, &key));
	key = 9;
	check(ptrs_void_lookup(cvh, &key));
	key = 5;
	check(ptrs_void_lookup(cvh, &key));
	check(3 == ptrs_void_elems(cvh));
	
	key = 9;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(ptrs_void_lookup(cvh, &key));
	key = 5;
	check(ptrs_void_lookup(cvh, &key));
	check(2 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhv_pool_head(cvh));
	}
	
	key = 1;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(ptrs_void_lookup(cvh, &key));
	check(1 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(NO_NEXT == node.next);
		
		check(0 == ptrhv_pool_head(cvh));
	}

	key = 5;
	check(ptrs_void_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrs_void_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrs_void_elems(cvh));
	check(ptrs_void_is_empty(cvh));
	
	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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

	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_remove_reinsert_list(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1},
		{5},
		{9},
	};
	
	ptrhv_ins(cvh, arr, 3);
	check(3 == ptrs_void_elems(cvh));
	check(!ptrs_void_is_empty(cvh));
	check(4 == ptrs_void_cap(cvh));
	
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
	check(1 == ptrhv_get_pool_index(cvh, node.next));

	check(ptrhv_get_pool_val(cvh, 1) == 9);
	check(ptrhv_get_pool_next(cvh, 1) == 0);
	check(ptrhv_get_pool_val(cvh, 0) == 5);
	check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key_0 = 10;
	check(ptrs_void_remove(cvh, &key_0) == cvh);
	
	key_0 = 1;
	check(ptrs_void_lookup(cvh, &key_0));
	key_0 = 5;
	check(ptrs_void_lookup(cvh, &key_0));
	key_0 = 9;
	check(ptrs_void_lookup(cvh, &key_0));
	check(3 == ptrs_void_elems(cvh));
	
	key_0 = 9;
	check(ptrs_void_remove(cvh, &key_0) == cvh);
	key_0 = 9;
	check(!ptrs_void_lookup(cvh, &key_0));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key_0 = 1;
	check(ptrs_void_lookup(cvh, &key_0));
	key_0 = 5;
	check(ptrs_void_lookup(cvh, &key_0));
	check(2 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhv_pool_head(cvh));
	}
	
	int key_1 = 9;
	ptrs_void_insert(cvh, &key_1);
	check(3 == ptrs_void_elems(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	
	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(1 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 1) == 9);
		check(ptrhv_get_pool_next(cvh, 1) == 0);
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == ptrhv_pool_head(cvh));
	}
	
	int key_2 = 1;
	check(ptrs_void_remove(cvh, &key_2) == cvh);
	key_2 = 1;
	check(!ptrs_void_lookup(cvh, &key_2));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key_2 = 9;
	check(ptrs_void_lookup(cvh, &key_2));
	key_2 = 5;
	check(ptrs_void_lookup(cvh, &key_2));
	check(2 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhv_pool_head(cvh));
	}
	
	int key_3 = 1;
	ptrs_void_insert(cvh, &key_3);
	check(3 == ptrs_void_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 1 -> 5 -> x
	
	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(1 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 1) == 1);
		check(ptrhv_get_pool_next(cvh, 1) == 0);
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == ptrhv_pool_head(cvh));
	}
	
	// repeat; remove from tail
	
	int key_4 = 5;
	check(ptrs_void_remove(cvh, &key_4) == cvh);
	key_4 = 5;
	check(!ptrs_void_lookup(cvh, &key_4));
	
	// 0 = x
	// 1 = 9 -> 1 -> x
	
	key_4 = 9;
	check(ptrs_void_lookup(cvh, &key_4));
	key_4 = 1;
	check(ptrs_void_lookup(cvh, &key_4));
	check(2 == ptrs_void_elems(cvh));

	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(1 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 1) == 1);
		check(ptrhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == ptrhv_pool_head(cvh));
	}
	
	int key_5 = 5;
	ptrs_void_insert(cvh, &key_5);
	check(3 == ptrs_void_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 5 -> 1 -> x
	
	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
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
		check(0 == ptrhv_get_pool_index(cvh, node.next));
		check(ptrhv_get_pool_val(cvh, 0) == 5);
		check(ptrhv_get_pool_next(cvh, 0) == 1);
		check(ptrhv_get_pool_val(cvh, 1) == 1);
		check(ptrhv_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(2 == ptrhv_pool_head(cvh));
	}
	
	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_remove_reinsert_trivial(void)
{
	ptrs_void cvh_, * cvh = &cvh_;
	
	check(ptrs_void_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	tbl_node node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	int key = 1;
	
	check(ptrs_void_insert(cvh, &key) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(ptrs_void_elems(cvh) == 1);
	check(!ptrs_void_is_empty(cvh));
	
	
	{
		ptrs_iterator ptrhv_iter_, * ptrhv_iter = &ptrhv_iter_;
		ptrs_iterator_init(ptrhv_iter);
		
		const void * cvh_key = NULL;
		void * cvh_val = NULL;
		
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_void_iterate(cvh, ptrhv_iter);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
	}
	
	node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(NO_NEXT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	check(ptrs_void_remove(cvh, &key) == cvh);
	check(ptrs_void_is_empty(cvh));
	
	node = ptrhv_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhv_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	ptrs_void_destroy(cvh);
	return true;
}

static bool test_ptrs_void_remove(void)
{
	check(test_ptrs_void_remove_from_front());
	check(test_ptrs_void_remove_from_tail());
	check(test_ptrs_void_remove_from_middle());
	check(test_ptrs_void_remove_reinsert_trivial());
	check(test_ptrs_void_remove_reinsert_list());
	return true;
}

PTR_SET_DEFINE(int, int);

static int ptrhid_get_pool_val(ptrs_int * ptrhid, size_t n)
{
	return ptrhv_get_pool_val(&ptrhid->tbl, n);
}
static size_t ptrhid_get_pool_next(ptrs_int * ptrhid, size_t n)
{
	return ptrhv_get_pool_next(&ptrhid->tbl, n);
}
static size_t ptrhid_get_pool_index(ptrs_int * ptrhid, const void * elem)
{
	return ptrhv_get_pool_index(&ptrhid->tbl, elem);
}
static void * ptrhid_get_tbl(ptrs_int * ptrhid)
{
	return ptrhv_get_tbl(&ptrhid->tbl);
}
static size_t ptrhid_pool_head(ptrs_int * ptrhid)
{
	return ptrhv_pool_head(&ptrhid->tbl);
}
static ptrs_compar ptrhid_get_cmp(ptrs_int * ptrhid)
{
	return ptrhv_get_cmp(&ptrhid->tbl);
}
static ptrs_get_hash ptrhid_get_hash(ptrs_int * ptrhid)
{
	return ptrhv_get_hash(&ptrhid->tbl);
}

static bool ptrhid_is_tbl_clear(ptrs_int * ptrhid)
{
	return ptrhv_is_tbl_clear(&ptrhid->tbl);
}
tbl_node ptrhid_get_tbl_node_at(ptrs_int * ptrhid, size_t ind)
{
	return ptrhv_get_tbl_node_at(&ptrhid->tbl, ind);
}
static bool ptrhid_test_copy(ptrs_int * ptrhid)
{
	check(ptrhv_test_copy(&ptrhid->tbl));
	return true;
}

static bool test_ptrs_int_create(void)
{
	zero_icmp_calls();
	zero_hash_calls();
	
	{
		ptrs_int cvh_, * cvh = &cvh_;
		check(ptrs_int_create_sz_pl(cvh, get_ihash, icmp, 3, 5) == cvh);
		
		check(ptrs_priv_pool_curr_cap(&cvh->tbl.chain_pool) == 5);
		check(0 == cvh->tbl.elems);
		check(ptrs_int_elems(cvh) == cvh->tbl.elems);
		check(ptrs_int_is_empty(cvh));
		check(ptrs_int_cap(cvh) == 4);
		
		check(ptrhid_is_tbl_clear(cvh));
		check(ptrhid_get_tbl(cvh));
		check(ptrhid_get_cmp(cvh) == icmp);
		check(ptrhid_get_hash(cvh) == get_ihash);
		
		check(ptrhid_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		ptrs_int_destroy(cvh);
	}
	
	{
		ptrs_int cvh_, * cvh = &cvh_;
		check(ptrs_int_create(cvh, get_ihash, icmp) == cvh);
		
		check(ptrs_priv_pool_curr_cap(&cvh->tbl.chain_pool)
			== PTR_SET_DEFAULT_POOL);
		check(0 == cvh->tbl.elems);
		check(ptrs_int_elems(cvh) == cvh->tbl.elems);
		check(ptrs_int_is_empty(cvh));
		check(ptrs_int_cap(cvh) == PTR_SET_DEFAULT_SIZE);
			
		check(ptrhid_is_tbl_clear(cvh));
		check(ptrhid_get_tbl(cvh));
		check(ptrhid_get_cmp(cvh) == icmp);
		check(ptrhid_get_hash(cvh) == get_ihash);
		
		check(ptrhid_test_copy(cvh));
		
		check(icmp_calls(0));
		check(hash_calls(0));
		
		ptrs_int_destroy(cvh);
	}
	
	return true;
}


static bool test_ptrs_int_insert(void)
{
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	int key_1 = 1;
	
	check(ptrs_int_insert(cvh, &key_1) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(ptrs_int_elems(cvh) == 1);
	check(!ptrs_int_is_empty(cvh));
	
	check(icmp_calls(0));
	check(hash_calls(1));
	
	tbl_node node = ptrhid_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	int key_2 = 5;
	check(ptrs_int_insert(cvh, &key_2) == cvh);
	// 0 = x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(ptrs_int_elems(cvh) == 2);
	
	check(icmp_calls(1));
	check(hash_calls(2));
	
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(0 == ptrhid_get_pool_index(cvh, node.next));
	
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key_3 = 4;
	check(ptrs_int_insert(cvh, &key_3) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = x
	check(ptrs_int_elems(cvh) == 3);
	
	check(icmp_calls(1));
	check(hash_calls(3));
	
	node = ptrhid_get_tbl_node_at(cvh, 0);
	check(&key_3 == node.key);
	check(4 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	int key_4 = 7;
	check(ptrs_int_insert(cvh, &key_4) == cvh);
	// 0 = 4 -> x
	// 1 = 1 -> 5 -> x
	// 2 = x
	// 3 = 7 -> x
	check(ptrs_int_elems(cvh) == 4);
	
	check(icmp_calls(1));
	check(hash_calls(4));
	
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(&key_4 == node.key);
	check(7 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	
	// growth event
	int key_5 = 12;
	check(ptrs_int_cap(cvh) == 4);
	check(ptrs_int_insert(cvh, &key_5) == cvh);
	check(ptrs_int_cap(cvh) == 8);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(ptrs_int_elems(cvh) == 5);
	
	check(icmp_calls(2));
	check(hash_calls(9));
	
	int key_6 = 20;
	check(ptrs_int_insert(cvh, &key_6) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	// 4 = 4 -> 20 -> 12 -> x
	// 5 = 5 -> x
	// 6 = x
	// 7 = 7 -> x
	check(ptrs_int_elems(cvh) == 6);
	
	check(icmp_calls(4));
	check(hash_calls(10));
	
	node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(&key_1 == node.key);
	check(1 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 4);
	check(&key_3 == node.key);
	check(4 == *(int *)node.key);
	check(1 == ptrhid_get_pool_index(cvh, node.next));
	
	check(ptrhid_get_pool_val(cvh, 1) == 20);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 12);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	node = ptrhid_get_tbl_node_at(cvh, 5);
	check(&key_2 == node.key);
	check(5 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 6);
	check(NOT_INIT == node.next);
	
	node = ptrhid_get_tbl_node_at(cvh, 7);
	check(&key_4 == node.key);
	check(7 == *(int *)node.key);
	check(NO_NEXT == node.next);
	
	ptrs_int_destroy(cvh);
	
	zero_hash_calls();
	zero_icmp_calls();
	
	check(icmp_calls(0));
	check(hash_calls(0));
	
	return true;
}

static ptrs_int * ptrhid_ins(ptrs_int * cvh, data_node * arr, size_t len)
{
	for (size_t i = 0; i < len; ++i)
		ptrs_int_insert(cvh, &arr[i].key);
	return cvh;
}
static bool ptrhid_get4(ptrs_int * cvh)
{
	static data_node arr[] = {
		{1},
		{5},
		{4},
		{7},
	};
	
	ptrhid_ins(cvh, arr, 4);
	check(4 == ptrs_int_elems(cvh));
	check(!ptrs_int_is_empty(cvh));
	return true;
}
static bool ptrhid_get6(ptrs_int * cvh)
{
	static data_node arr[] = {
		{12},
		{20},
	};
	
	ptrhid_get4(cvh);
	ptrhid_ins(cvh, arr, 2);
	check(6 == ptrs_int_elems(cvh));
	check(!ptrs_int_is_empty(cvh));
	return true;
}

static bool test_ptrs_int_iterate(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhid_get4(cvh);
	
	ptrs_iterator iter_, * iter = &iter_;
	ptrs_iterator_init(iter);
	
	const int * key = NULL;
	double * val = NULL;
	
	key = ptrs_int_iterate(cvh, iter);
	check(4 == *(int *)key);
	key = ptrs_int_iterate(cvh, iter);
	check(1 == *(int *)key);
	key = ptrs_int_iterate(cvh, iter);
	check(5 == *(int *)key);
	key = ptrs_int_iterate(cvh, iter);
	check(7 == *(int *)key);
	
	key = ptrs_int_iterate(cvh, iter);
	check(NULL == key);
	check(NULL == val);
	
	ptrhid_get6(cvh);
	
	ptrs_iterator_init(iter);
	key = ptrs_int_iterate(cvh, iter);
	check(1 == *(int *)key);
	key = ptrs_int_iterate(cvh, iter);
	check(4 == *(int *)key);
	key = ptrs_int_iterate(cvh, iter);
	check(20 == *(int *)key);
	key = ptrs_int_iterate(cvh, iter);
	check(12 == *(int *)key);
	key = ptrs_int_iterate(cvh, iter);
	check(5 == *(int *)key);
	key = ptrs_int_iterate(cvh, iter);
	check(7 == *(int *)key);
	
	key = ptrs_int_iterate(cvh, iter);
	check(NULL == key);
	check(NULL == val);
	
	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_rewrite(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhid_get4(cvh);
	
	int key_1 = 1;
	check(ptrs_int_insert(cvh, &key_1) == cvh);
	
	int key_2 = 4;
	check(ptrs_int_insert(cvh, &key_2) == cvh);
	
	int key_3 = 5;
	check(ptrs_int_insert(cvh, &key_3) == cvh);
		
	ptrs_iterator iter_, * iter = &iter_;
	ptrs_iterator_init(iter);
	
	const int * pkey = NULL;
		
	pkey = ptrs_int_iterate(cvh, iter);
	check(4 == *(int *)pkey);
	pkey = ptrs_int_iterate(cvh, iter);
	check(1 == *(int *)pkey);
	pkey = ptrs_int_iterate(cvh, iter);
	check(5 == *(int *)pkey);
	pkey = ptrs_int_iterate(cvh, iter);
	check(7 == *(int *)pkey);
	
	pkey = ptrs_int_iterate(cvh, iter);
	check(NULL == pkey);
		
	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_lookup(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	int key = 4;
	const int * pkey = NULL;
	check(NULL == ptrs_int_lookup(cvh, &key));
	
	key = 0;
	check(NULL == ptrs_int_lookup(cvh, &key));
	
	ptrhid_get4(cvh);
	key = 4;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 1;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 1000;
	check(NULL == ptrs_int_lookup(cvh, &key));
	
	key = 5;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 7;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = -1000;
	check(NULL == ptrs_int_lookup(cvh, &key));
	
	ptrhid_get6(cvh);
	
	key = 5;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 20;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_clear(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	ptrhid_get4(cvh);
	
	int key = 4;
	const int * pkey = NULL;
	
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 1;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 5;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 7;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	size_t h_calls = get_hash_calls();
	size_t cmp_calls = get_icmp_calls();
	check(ptrs_int_clear(cvh) == cvh);
	check(0 == ptrs_int_elems(cvh));
	check(ptrs_int_is_empty(cvh));
	hash_calls(h_calls);
	icmp_calls(cmp_calls);
	
	key = 4;
	pkey = ptrs_int_lookup(cvh, &key);
	
	key = 1;
	pkey = ptrs_int_lookup(cvh, &key);
	
	key = 5;
	pkey = ptrs_int_lookup(cvh, &key);
	
	key = 7;
	pkey = ptrs_int_lookup(cvh, &key);
	
	ptrhid_get6(cvh);
	
	key = 1;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 5;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 7;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 4;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 20;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	key = 12;
	pkey = ptrs_int_lookup(cvh, &key);
	check(*pkey == key);
	
	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_copy(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);
	
	check(ptrhid_test_copy(cvh));
	
	ptrhid_get4(cvh);
	check(ptrhid_test_copy(cvh));
	
	ptrhid_get6(cvh);
	check(ptrhid_test_copy(cvh));
	
	ptrs_int_destroy(cvh);
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 5) == cvh);
	
	ptrhid_get6(cvh);
	check(ptrhid_test_copy(cvh));
	
	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_remove_from_front(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1},
		{5},
		{9},
	};

	ptrhid_ins(cvh, arr, 3);
	check(3 == ptrs_int_elems(cvh));
	check(!ptrs_int_is_empty(cvh));
	check(4 == ptrs_int_cap(cvh));
	
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
	check(1 == ptrhid_get_pool_index(cvh, node.next));
	
	check(ptrhid_get_pool_val(cvh, 1) == 9);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(ptrs_int_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrs_int_lookup(cvh, &key));
	key = 5;
	check(ptrs_int_lookup(cvh, &key));
	key = 9;
	check(ptrs_int_lookup(cvh, &key));
	check(3 == ptrs_int_elems(cvh));
	
	key = 1;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key = 9;
	check(ptrs_int_lookup(cvh, &key));
	key = 5;
	check(ptrs_int_lookup(cvh, &key));
	check(2 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh,0) == NO_NEXT_N);
		
		check(1 == ptrhid_pool_head(cvh));
	}
	
	key = 9;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(ptrs_int_lookup(cvh, &key));
	check(1 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(NO_NEXT == node.next);
		
		check(0 == ptrhid_pool_head(cvh));
	}

	key = 5;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrs_int_elems(cvh));
	check(ptrs_int_is_empty(cvh));
	
	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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

	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_remove_from_tail(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1},
		{5},
		{9},
	};
	
	ptrhid_ins(cvh, arr, 3);
	check(3 == ptrs_int_elems(cvh));
	check(!ptrs_int_is_empty(cvh));
	check(4 == ptrs_int_cap(cvh));
	
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
	check(1 == ptrhid_get_pool_index(cvh, node.next));

	check(ptrhid_get_pool_val(cvh, 1) == 9);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key = 10;
	check(ptrs_int_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrs_int_lookup(cvh, &key));
	key = 5;
	check(ptrs_int_lookup(cvh, &key));
	key = 9;
	check(ptrs_int_lookup(cvh, &key));
	check(3 == ptrs_int_elems(cvh));
	
	key = 5;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 9 -> x
	
	key = 1;
	check(ptrs_int_lookup(cvh, &key));
	key = 9;
	check(ptrs_int_lookup(cvh, &key));
	check(2 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(1 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 1) == 9);
		check(ptrhid_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == ptrhid_pool_head(cvh));
	}
	
	key = 9;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> x
	
	key = 1;
	check(ptrs_int_lookup(cvh, &key));
	check(1 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(NO_NEXT == node.next);
		
		check(1 == ptrhid_pool_head(cvh));
	}

	key = 1;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrs_int_elems(cvh));
	check(ptrs_int_is_empty(cvh));
	
	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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

	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_remove_from_middle(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1},
		{5},
		{9},
	};
	
	ptrhid_ins(cvh, arr, 3);
	check(3 == ptrs_int_elems(cvh));
	check(!ptrs_int_is_empty(cvh));
	check(4 == ptrs_int_cap(cvh));
	
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
	check(1 == ptrhid_get_pool_index(cvh, node.next));

	check(ptrhid_get_pool_val(cvh, 1) == 9);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
	
	int key = 10;
	check(ptrs_int_remove(cvh, &key) == cvh);
	
	key = 1;
	check(ptrs_int_lookup(cvh, &key));
	key = 9;
	check(ptrs_int_lookup(cvh, &key));
	key = 5;
	check(ptrs_int_lookup(cvh, &key));
	check(3 == ptrs_int_elems(cvh));
	
	key = 9;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 9;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key = 1;
	check(ptrs_int_lookup(cvh, &key));
	key = 5;
	check(ptrs_int_lookup(cvh, &key));
	check(2 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhid_pool_head(cvh));
	}
	
	key = 1;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 1;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = 5 -> x
	
	key = 5;
	check(ptrs_int_lookup(cvh, &key));
	check(1 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(NO_NEXT == node.next);
		
		check(0 == ptrhid_pool_head(cvh));
	}

	key = 5;
	check(ptrs_int_remove(cvh, &key) == cvh);
	key = 5;
	check(!ptrs_int_lookup(cvh, &key));
	
	// 0 = x
	// 1 = x
	
	check(0 == ptrs_int_elems(cvh));
	check(ptrs_int_is_empty(cvh));
	
	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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

	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_remove_reinsert_list(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);

	data_node arr[] = {
		{1},
		{5},
		{9},
	};
	
	ptrhid_ins(cvh, arr, 3);
	check(3 == ptrs_int_elems(cvh));
	check(!ptrs_int_is_empty(cvh));
	check(4 == ptrs_int_cap(cvh));
	
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
	check(1 == ptrhid_get_pool_index(cvh, node.next));

	check(ptrhid_get_pool_val(cvh, 1) == 9);
	check(ptrhid_get_pool_next(cvh, 1) == 0);
	check(ptrhid_get_pool_val(cvh, 0) == 5);
	check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);

	int key_0 = 10;
	check(ptrs_int_remove(cvh, &key_0) == cvh);
	
	key_0 = 1;
	check(ptrs_int_lookup(cvh, &key_0));
	key_0 = 5;
	check(ptrs_int_lookup(cvh, &key_0));
	key_0 = 9;
	check(ptrs_int_lookup(cvh, &key_0));
	check(3 == ptrs_int_elems(cvh));
	
	key_0 = 9;
	check(ptrs_int_remove(cvh, &key_0) == cvh);
	key_0 = 9;
	check(!ptrs_int_lookup(cvh, &key_0));
	
	// 0 = x
	// 1 = 1 -> 5 -> x
	
	key_0 = 1;
	check(ptrs_int_lookup(cvh, &key_0));
	key_0 = 5;
	check(ptrs_int_lookup(cvh, &key_0));
	check(2 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhid_pool_head(cvh));
	}
	
	int key_1 = 9;
	ptrs_int_insert(cvh, &key_1);
	check(3 == ptrs_int_elems(cvh));
	
	// 0 = x
	// 1 = 1 -> 9 -> 5 -> x
	
	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(1 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 1) == 9);
		check(ptrhid_get_pool_next(cvh, 1) == 0);
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == ptrhid_pool_head(cvh));
	}
	
	int key_2 = 1;
	check(ptrs_int_remove(cvh, &key_2) == cvh);
	key_2 = 1;
	check(!ptrs_int_lookup(cvh, &key_2));
	
	// 0 = x
	// 1 = 9 -> 5 -> x
	
	key_2 = 9;
	check(ptrs_int_lookup(cvh, &key_2));
	key_2 = 5;
	check(ptrs_int_lookup(cvh, &key_2));
	check(2 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(1 == ptrhid_pool_head(cvh));
	}
	
	int key_3 = 1;
	ptrs_int_insert(cvh, &key_3);
	check(3 == ptrs_int_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 1 -> 5 -> x
	
	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(1 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 1) == 1);
		check(ptrhid_get_pool_next(cvh, 1) == 0);
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == NO_NEXT_N);
		
		check(2 == ptrhid_pool_head(cvh));
	}
	
	// repeat; remove from tail
	
	int key_4 = 5;
	check(ptrs_int_remove(cvh, &key_4) == cvh);
	key_4 = 5;
	check(!ptrs_int_lookup(cvh, &key_4));
	
	// 0 = x
	// 1 = 9 -> 1 -> x
	
	key_4 = 9;
	check(ptrs_int_lookup(cvh, &key_4));
	key_4 = 1;
	check(ptrs_int_lookup(cvh, &key_4));
	check(2 == ptrs_int_elems(cvh));

	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(1 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 1) == 1);
		check(ptrhid_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(0 == ptrhid_pool_head(cvh));
	}
	
	int key_5 = 5;
	ptrs_int_insert(cvh, &key_5);
	check(3 == ptrs_int_elems(cvh));
	
	// 0 = x
	// 1 = 9 -> 5 -> 1 -> x
	
	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 9);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 5);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
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
		check(0 == ptrhid_get_pool_index(cvh, node.next));
		check(ptrhid_get_pool_val(cvh, 0) == 5);
		check(ptrhid_get_pool_next(cvh, 0) == 1);
		check(ptrhid_get_pool_val(cvh, 1) == 1);
		check(ptrhid_get_pool_next(cvh, 1) == NO_NEXT_N);
		
		check(2 == ptrhid_pool_head(cvh));
	}
	
	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_remove_reinsert_trivial(void)
{
	ptrs_int cvh_, * cvh = &cvh_;
	
	check(ptrs_int_create_sz(cvh, get_ihash, icmp, 4) == cvh);
		
	tbl_node node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	int key = 1;
	
	check(ptrs_int_insert(cvh, &key) == cvh);
	// 0 = x
	// 1 = 1 -> x
	// 2 = x
	// 3 = x
	check(ptrs_int_elems(cvh) == 1);
	check(!ptrs_int_is_empty(cvh));
	
	
	{
		ptrs_iterator ptrhid_iter_, * ptrhid_iter = &ptrhid_iter_;
		ptrs_iterator_init(ptrhid_iter);
		
		const int * cvh_key = NULL;
		double * cvh_val = NULL;
		
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check((int)(*(int *)cvh_key) == 1);
		cvh_key = ptrs_int_iterate(cvh, ptrhid_iter);
		check(NULL == cvh_key);
		check(NULL == cvh_val);
	}
	
	node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(1 == *(int *)node.key);
	check(NO_NEXT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	check(ptrs_int_remove(cvh, &key) == cvh);
	check(ptrs_int_is_empty(cvh));
	
	node = ptrhid_get_tbl_node_at(cvh, 0);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 1);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 2);
	check(NOT_INIT == node.next);
	node = ptrhid_get_tbl_node_at(cvh, 3);
	check(NOT_INIT == node.next);
	
	ptrs_int_destroy(cvh);
	return true;
}

static bool test_ptrs_int_remove(void)
{
	check(test_ptrs_int_remove_from_front());
	check(test_ptrs_int_remove_from_tail());
	check(test_ptrs_int_remove_from_middle());
	check(test_ptrs_int_remove_reinsert_trivial());
	check(test_ptrs_int_remove_reinsert_list());
	return true;
}


PTR_SET_DEFINE(pint, int *);

static bool test_ptrs_ptrs(void)
{
	ptrs_pint tbl_, * tbl = &tbl_;
	
	check(ptrs_pint_create_sz(tbl, get_ihash, icmp, 4) == tbl);
	
	int foo = 0;
	const int * pfoo = &foo;
	
	check(ptrs_pint_insert(tbl, &pfoo) == tbl);
	check(*ptrs_pint_lookup(tbl, &pfoo) == pfoo);
	
	ptrs_pint_destroy(tbl);
	return true;
}

typedef tbl_node pool_node;

static ptrs_node * ptrht_priv_pool_reach(ptrs_priv_pool * pool, size_t n)
{
	return ptrs_priv_get_elem_addr(pool, n);
}
static size_t ptrht_priv_pool_index(ptrs_priv_pool * pool, const void * elem)
{
	ptrs_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return ptrhv_get_pool_index(tbl, elem);
}
static size_t ptrht_priv_pool_get_next(ptrs_priv_pool * pool, size_t n)
{
	ptrs_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return ptrhv_get_pool_next(tbl, n);
}
static size_t ptrht_priv_pool_head(ptrs_priv_pool * pool)
{
	ptrs_void tbl_, * tbl = &tbl_;
	tbl->chain_pool = *pool;
	return ptrhv_pool_head(tbl);
}

#define to_pvoid(x) ((void *)(x))
static bool test_ptrs_set_priv_pool(void)
{
	ptrs_priv_pool pool_, * pool = &pool_;
	
	check(ptrs_priv_pool_create(pool, 2) == pool);
	check(2 == ptrs_priv_pool_curr_cap(pool));
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
	elem_a = ptrs_priv_pool_get(pool);
	elem_a_ind = ptrht_priv_pool_index(pool, elem_a);
	check(0 == elem_a_ind);
	check(1 == ptrht_priv_pool_head(pool));
	ptrht_priv_pool_reach(pool, elem_a_ind)->key = to_pvoid(100);
	check(to_pvoid(100) == ptrht_priv_pool_reach(pool, elem_a_ind)->key);
	
	void * elem_b = NULL;
	size_t elem_b_ind = NO_NEXT_N;
	elem_b = ptrs_priv_pool_get(pool);
	elem_b_ind = ptrht_priv_pool_index(pool, elem_b);
	check(1 == elem_b_ind);
	check(NO_NEXT_N == ptrht_priv_pool_head(pool));
	ptrht_priv_pool_reach(pool, elem_b_ind)->key = to_pvoid(200);
	check(to_pvoid(200) == ptrht_priv_pool_reach(pool, elem_b_ind)->key);
	
	void * elem_c = ptrs_priv_pool_get(pool);
	size_t elem_c_ind = ptrht_priv_pool_index(pool, elem_c);
	check(0 == elem_c_ind);
	check(2 == ptrs_priv_pool_curr_cap(pool));
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
	
	ptrs_priv_pool_destroy(pool);
	return true;
}

static ftest tests[] = {
	test_ptrs_void_create,
	test_ptrs_void_insert,
	test_ptrs_void_iterate,
	test_ptrs_void_rewrite,
	test_ptrs_void_lookup,
	test_ptrs_void_clear,
	test_ptrs_void_copy,
	test_ptrs_void_remove,
	test_ptrs_int_create,
	test_ptrs_int_insert,
	test_ptrs_int_iterate,
	test_ptrs_int_rewrite,
	test_ptrs_int_lookup,
	test_ptrs_int_clear,
	test_ptrs_int_copy,
	test_ptrs_int_remove,
	test_ptrs_ptrs,
	test_ptrs_set_priv_pool,
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
