#include <stdio.h>
#include "test.h"
#include "pool/pool.h"

#define UNUSED(x) (void)(x)

static bool test_pool_void_create_no_cbks_allign(void)
{
	{
		pool_void pool_, * pool = &pool_;
		nslist_void * free_list = NULL;
		nslist_void * pool_list = NULL;
		void * node = NULL;
		void ** pnext = NULL;
		size_t addr = 0;
		
		typedef struct pool_elem {
			char ch;
		} pool_elem;
		
		check(pool_void_create_pod(pool, sizeof(pool_elem), 3) == pool);
		
		free_list = &pool->free_list;
		pool_list = &pool->pool_list;
		
		check(sizeof(void*)*2*3 == pool_list->next_offs);
		check(sizeof(void*) == free_list->next_offs);
		node = nslist_void_head(free_list);
		check(node);
		
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		check(!node);
		
		pool_void_destroy(pool);
	}
	
	{
		pool_void pool_, * pool = &pool_;
		nslist_void * free_list = NULL;
		void * node = NULL;
		void ** pnext = NULL;
		size_t addr = 0;
		
		typedef struct pool_elem {
			char ch[sizeof(void*)-1];
		} pool_elem;
		
		check(pool_void_create_pod(pool, sizeof(pool_elem), 3) == pool);
		
		free_list = &pool->free_list;
		
		check(sizeof(void*) == free_list->next_offs);
		node = nslist_void_head(free_list);
		check(node);
		
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		check(!node);
		
		pool_void_destroy(pool);
	}
	
	{
		pool_void pool_, * pool = &pool_;
		nslist_void * free_list = NULL;
		void * node = NULL;
		void ** pnext = NULL;
		size_t addr = 0;
		
		typedef struct pool_elem {
			char ch[sizeof(void*)];
		} pool_elem;
		
		check(pool_void_create_pod(pool, sizeof(pool_elem), 3) == pool);
		
		free_list = &pool->free_list;
		
		check(sizeof(void*) == free_list->next_offs);
		node = nslist_void_head(free_list);
		check(node);
		
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		check(!node);
		
		pool_void_destroy(pool);
	}
	
	{
		pool_void pool_, * pool = &pool_;
		nslist_void * free_list = NULL;
		void * node = NULL;
		void ** pnext = NULL;
		size_t addr = 0;
		
		typedef struct pool_elem {
			char ch[sizeof(void*)+1];
		} pool_elem;
		
		check(pool_void_create_pod(pool, sizeof(pool_elem), 3) == pool);
		
		free_list = &pool->free_list;
		
		check(sizeof(void*)*2 == free_list->next_offs);
		node = nslist_void_head(free_list);
		check(node);
		
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		check(!node);
		
		pool_void_destroy(pool);
	}
	
	{
		pool_void pool_, * pool = &pool_;
		nslist_void * free_list = NULL;
		void * node = NULL;
		void ** pnext = NULL;
		size_t addr = 0;
		
		typedef struct pool_elem {
			char ch[sizeof(void*)*2-1];
		} pool_elem;
		
		check(pool_void_create_pod(pool, sizeof(pool_elem), 3) == pool);
		
		free_list = &pool->free_list;
		
		check(sizeof(void*)*2 == free_list->next_offs);
		node = nslist_void_head(free_list);
		check(node);
		
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		check(!node);
		
		pool_void_destroy(pool);
	}
	
	{
		pool_void pool_, * pool = &pool_;
		nslist_void * free_list = NULL;
		void * node = NULL;
		void ** pnext = NULL;
		size_t addr = 0;
		
		typedef struct pool_elem {
			char ch[sizeof(void*)*2];
		} pool_elem;
		
		check(pool_void_create_pod(pool, sizeof(pool_elem), 3) == pool);
		
		free_list = &pool->free_list;
		
		check(sizeof(void*)*2 == free_list->next_offs);
		node = nslist_void_head(free_list);
		check(node);
		
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		check(!node);
		
		pool_void_destroy(pool);
	}
	
	{
		pool_void pool_, * pool = &pool_;
		nslist_void * free_list = NULL;
		void * node = NULL;
		void ** pnext = NULL;
		size_t addr = 0;
		
		typedef struct pool_elem {
			char ch[sizeof(void*)*2+1];
		} pool_elem;
		
		check(pool_void_create_pod(pool, sizeof(pool_elem), 3) == pool);
		
		free_list = &pool->free_list;
		
		check(sizeof(void*)*3 == free_list->next_offs);
		node = nslist_void_head(free_list);
		check(node);
		
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		pnext = nslist_void_pnext(free_list, node);
		addr = (size_t)pnext;
		check((addr % sizeof(void*)) == 0);
		
		node = *pnext;
		check(!node);
		
		pool_void_destroy(pool);
	}
	
	return true;
}

typedef struct node_int {
	int n;
} node_int;

static int g_dtor_called = 0;

static void peint_ctor(void * elem, void * arg)
{
	node_int * pei = (node_int *)elem;
	pei->n = ++*(int *)arg;
}

static void peint_dtor(void * elem)
{
	UNUSED(elem);
	++g_dtor_called;
}

static bool test_pool_void_create(void)
{
	pool_void pool_, * pool = &pool_;
	nslist_void * free_list = NULL;
	nslist_void * pool_list = NULL;
	void * node = NULL;
	void ** pnext = NULL;
	size_t addr = 0;
	
	int arg = 0;
	check(pool_void_create(pool, sizeof(node_int), 3,
		peint_ctor, &arg, peint_dtor) == pool);
	
	check(3 == arg);
	
	free_list = &pool->free_list;
	pool_list = &pool->pool_list;
	
	check(sizeof(void*)*3*2 == pool_list->next_offs);
	check(sizeof(void*) == free_list->next_offs);
	node = nslist_void_head(free_list);
	check(node);
	check(1 == ((node_int *)node)->n);
	
	pnext = nslist_void_pnext(free_list, node);
	addr = (size_t)pnext;
	check((addr % sizeof(void*)) == 0);
	
	node = *pnext;
	check(2 == ((node_int *)node)->n);
	pnext = nslist_void_pnext(free_list, node);
	addr = (size_t)pnext;
	check((addr % sizeof(void*)) == 0);
	
	node = *pnext;
	check(3 == ((node_int *)node)->n);
	pnext = nslist_void_pnext(free_list, node);
	addr = (size_t)pnext;
	check((addr % sizeof(void*)) == 0);
	
	node = *pnext;
	check(!node);
	
	void * mem = NULL;
	void * mem_next = NULL;
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	node_int * pei_a = NULL;
	pei_a = (node_int *)pool_void_get(pool);
	check(pei_a);
	check(1 == pei_a->n);
	
	node = nslist_void_head(free_list);
	check(node);
	check(2 == ((node_int *)node)->n);
	
	node_int * pei_b = NULL;
	pei_b = (node_int *)pool_void_get(pool);
	check(pei_b);
	check(2 == pei_b->n);
	
	node = nslist_void_head(free_list);
	check(node);
	check(3 == ((node_int *)node)->n);
	
	node_int * pei_c = NULL;
	pei_c = (node_int *)pool_void_get(pool);
	check(pei_c);
	check(3 == pei_c->n);
	
	node = nslist_void_head(free_list);
	check(!node);

	mem = nslist_void_head(pool_list);
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	node_int * pei_d = NULL;
	pei_d = (node_int *)pool_void_get(pool);
	check(pei_d);
	check(4 == pei_d->n);
	
	node = nslist_void_head(free_list);
	check(node);
	check(5 == ((node_int *)node)->n);
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(4 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(mem_next);
	mem = mem_next;
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	node_int * pei_e = NULL;
	pei_e = (node_int *)pool_void_get(pool);
	check(pei_e);
	check(5 == pei_e->n);
	
	node = nslist_void_head(free_list);
	check(node);
	check(6 == ((node_int *)node)->n);
	
	node_int * pei_f = NULL;
	pei_f = (node_int *)pool_void_get(pool);
	check(pei_f);
	check(6 == pei_f->n);
	
	node = nslist_void_head(free_list);
	check(!node);
	
	pool_void_put_back(pool, pei_a);
	node = nslist_void_head(free_list);
	check(node);
	check(1 == ((node_int *)node)->n);
	
	pool_void_put_back(pool, pei_b);
	node = nslist_void_head(free_list);
	check(node);
	check(2 == ((node_int *)node)->n);
	
	pool_void_put_back(pool, pei_c);
	node = nslist_void_head(free_list);
	check(node);
	check(3 == ((node_int *)node)->n);
	
	pool_void_put_back(pool, pei_f);
	node = nslist_void_head(free_list);
	check(node);
	check(6 == ((node_int *)node)->n);
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(4 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(mem_next);
	mem = mem_next;
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	pool_void_put_back(pool, pei_e);
	node = nslist_void_head(free_list);
	check(node);
	check(5 == ((node_int *)node)->n);
	
	pool_void_put_back(pool, pei_d);
	node = nslist_void_head(free_list);
	check(node);
	check(4 == ((node_int *)node)->n);
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(4 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(mem_next);
	mem = mem_next;
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	check(0 == g_dtor_called);
	pool_void_destroy(pool);
	check(6 == g_dtor_called);
	
	return true;
}

static bool test_pool_void_destroy(void)
{
	g_dtor_called = 0;
	check(0 == g_dtor_called);
	
	pool_void pool_, * pool = &pool_;
	
	int arg = 0;
	check(pool_void_create(pool, sizeof(node_int), 3,
		peint_ctor, &arg, peint_dtor) == pool);
	
	check(3 == arg);
	
	check(pool_void_get(pool));
	
	check(0 == g_dtor_called);
	pool_void_destroy(pool);
	check(3 == g_dtor_called);
	
	return true;
}

POOL_DEFINE(intn, node_int);

static bool test_pool_intn(void)
{
	g_dtor_called = 0;
	check(0 == g_dtor_called);
	
	pool_intn pool_, * pool = &pool_;
	nslist_void * free_list = NULL;
	nslist_void * pool_list = NULL;
	void * node = NULL;
	void ** pnext = NULL;
	size_t addr = 0;
	
	{
		pool_intn pool_, * pool = &pool_;
		int arg = 0;
		check(pool_intn_create_pod(pool, 3) == pool);
		check(0 == arg);
		pool_intn_destroy(pool);
	}
	
	int arg = 0;
	check(pool_intn_create(pool, 3,
		peint_ctor, &arg, peint_dtor) == pool);
	
	check(3 == arg);
	
	free_list = &pool->pool.free_list;
	pool_list = &pool->pool.pool_list;
	
	check(sizeof(void*)*3*2 == pool_list->next_offs);
	check(sizeof(void*) == free_list->next_offs);
	node = nslist_void_head(free_list);
	check(node);
	check(1 == ((node_int *)node)->n);
	
	pnext = nslist_void_pnext(free_list, node);
	addr = (size_t)pnext;
	check((addr % sizeof(void*)) == 0);
	
	node = *pnext;
	check(2 == ((node_int *)node)->n);
	pnext = nslist_void_pnext(free_list, node);
	addr = (size_t)pnext;
	check((addr % sizeof(void*)) == 0);
	
	node = *pnext;
	check(3 == ((node_int *)node)->n);
	pnext = nslist_void_pnext(free_list, node);
	addr = (size_t)pnext;
	check((addr % sizeof(void*)) == 0);
	
	node = *pnext;
	check(!node);
	
	void * mem = NULL;
	void * mem_next = NULL;
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	node_int * pei_a = NULL;
	pei_a = pool_intn_get(pool);
	check(pei_a);
	check(1 == pei_a->n);
	
	node = nslist_void_head(free_list);
	check(node);
	check(2 == ((node_int *)node)->n);
	
	node_int * pei_b = NULL;
	pei_b = pool_intn_get(pool);
	check(pei_b);
	check(2 == pei_b->n);
	
	node = nslist_void_head(free_list);
	check(node);
	check(3 == ((node_int *)node)->n);
	
	node_int * pei_c = NULL;
	pei_c = pool_intn_get(pool);
	check(pei_c);
	check(3 == pei_c->n);
	
	node = nslist_void_head(free_list);
	check(!node);
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	node_int * pei_d = NULL;
	pei_d = pool_intn_get(pool);
	check(pei_d);
	check(4 == pei_d->n);
	
	node = nslist_void_head(free_list);
	check(node);
	check(5 == ((node_int *)node)->n);
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(4 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(mem_next);
	mem = mem_next;
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	node_int * pei_e = NULL;
	pei_e = pool_intn_get(pool);
	check(pei_e);
	check(5 == pei_e->n);
	
	node = nslist_void_head(free_list);
	check(node);
	check(6 == ((node_int *)node)->n);
	
	node_int * pei_f = NULL;
	pei_f = pool_intn_get(pool);
	check(pei_f);
	check(6 == pei_f->n);
	
	node = nslist_void_head(free_list);
	check(!node);
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(4 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(mem_next);
	mem = mem_next;
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	pool_intn_put_back(pool, pei_a);
	node = nslist_void_head(free_list);
	check(node);
	check(1 == ((node_int *)node)->n);
	
	pool_intn_put_back(pool, pei_b);
	node = nslist_void_head(free_list);
	check(node);
	check(2 == ((node_int *)node)->n);
	
	pool_intn_put_back(pool, pei_c);
	node = nslist_void_head(free_list);
	check(node);
	check(3 == ((node_int *)node)->n);
	
	pool_intn_put_back(pool, pei_d);
	node = nslist_void_head(free_list);
	check(node);
	check(4 == ((node_int *)node)->n);
	
	pool_intn_put_back(pool, pei_e);
	node = nslist_void_head(free_list);
	check(node);
	check(5 == ((node_int *)node)->n);
	
	pool_intn_put_back(pool, pei_f);
	node = nslist_void_head(free_list);
	check(node);
	check(6 == ((node_int *)node)->n);
	
	mem = nslist_void_head(pool_list);
	check(mem);
	check(4 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(mem_next);
	mem = mem_next;
	check(mem);
	check(1 == ((node_int *)mem)->n);
	mem_next = *nslist_void_pnext(pool_list, mem);
	check(!mem_next);
	
	check(0 == g_dtor_called);
	pool_intn_destroy(pool);
	check(6 == g_dtor_called);
	
	return true;
}

static bool test_pool_intn_destroy(void)
{
	g_dtor_called = 0;
	check(0 == g_dtor_called);
	
	pool_intn pool_, * pool = &pool_;
	
	int arg = 0;
	check(pool_intn_create(pool, 3, peint_ctor, &arg, peint_dtor) == pool);
	
	check(3 == arg);
	
	check(pool_intn_get(pool));
	
	check(0 == g_dtor_called);
	pool_intn_destroy(pool);
	check(3 == g_dtor_called);
	
	return true;
}

static ftest tests[] = {
	test_pool_void_create_no_cbks_allign,
	test_pool_void_create,
	test_pool_void_destroy,
	test_pool_intn,
	test_pool_intn_destroy,
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
