#include <stdio.h>
#include "test.h"

#include <stdlib.h>
#include "grow_pool/grow_pool.h"
#include "grow_pool/grow_pool_priv.h"

typedef struct plnode {
	unsigned char n;
} plnode;

typedef struct plnode_true {
	plnode node;
	size_t next;
} plnode_true;

typedef struct plctor_arg {
	size_t count;
	int add;
} plctor_arg;

#define CTOR_COUNT_INIT 0
static void ctor(void * elem, void * arg)
{
	plctor_arg * parg = (plctor_arg *)arg;
	plnode * pelem = (plnode *)elem;
	pelem->n = parg->add + parg->count++;
}

#define UNUSED(x) (void)(x)

static size_t g_dtor_count = 0;
static void dtor(void * elem)
{
	UNUSED(elem);
	++g_dtor_count;
}

static bool test_gpool_void_create(void)
{
	grow_pool_void pool_, * pool = &pool_;
	vect_void * vect = &pool->vect;

	{
		check(gpool_void_create_pod(pool, sizeof(plnode), 3) == pool);
		check(vect_void_data(vect));
		check(sizeof(size_t)*2 == vect_void_elem_size(vect));
		check(0 == vect_void_len(vect));
		check(3 == vect_void_cap(vect));

		check(!pool->ctor);
		check(!pool->ctor_arg);
		check(!pool->dtor);

		plnode_true * node = NULL;
		node = (plnode_true *)vect_void_get(vect, 0);
		check(1 == node->next);
		node = (plnode_true *)vect_void_get(vect, 1);
		check(2 == node->next);
		node = (plnode_true *)vect_void_get(vect, 2);
		check(GPOOL_NO_NEXT == node->next);

		gpool_void_destroy(pool);
	}

	{
		plctor_arg ctor_count = {CTOR_COUNT_INIT, 100};

		check(gpool_void_create_sz(pool,
				sizeof(plnode), 3, ctor, &ctor_count, dtor) == pool);
		check(vect_void_data(vect));
		check(sizeof(size_t)*2 == vect_void_elem_size(vect));
		check(0 == vect_void_len(vect));
		check(3 == vect_void_cap(vect));

		check((fvoid)ctor == pool->ctor);
		check((void *)&ctor_count == pool->ctor_arg);
		check((fvoid)dtor == pool->dtor);

		plnode_true * node = NULL;
		node = (plnode_true *)vect_void_get(vect, 0);
		check(100 == node->node.n);
		check(1 == node->next);
		node = (plnode_true *)vect_void_get(vect, 1);
		check(101 == node->node.n);
		check(2 == node->next);
		node = (plnode_true *)vect_void_get(vect, 2);
		check(102 == node->node.n);
		check(GPOOL_NO_NEXT == node->next);

		check((3+CTOR_COUNT_INIT) == ctor_count.count);

		check(0 == g_dtor_count);
		gpool_void_destroy(pool);
		check(3 == g_dtor_count);
		g_dtor_count = 0;
		check(0 == g_dtor_count);
	}

	return true;
}

static bool test_gpool_void_use(void)
{
	grow_pool_void pool_, * pool = &pool_;
	vect_void * vect = &pool->vect;

	plctor_arg ctor_count = {CTOR_COUNT_INIT, 100};

	check(gpool_void_create_sz(pool,
			sizeof(plnode), 3, ctor, &ctor_count, dtor) == pool);
	check(vect_void_data(vect));
	check(sizeof(size_t)*2 == vect_void_elem_size(vect));
	check(0 == vect_void_len(vect));
	check(3 == vect_void_cap(vect));

	check((fvoid)ctor == pool->ctor);
	check((void *)&ctor_count == pool->ctor_arg);
	check((fvoid)dtor == pool->dtor);

	check((3+CTOR_COUNT_INIT) == ctor_count.count);

	plnode * pl_node = NULL;
	plnode_true * node = NULL;
	node = (plnode_true *)vect_void_get(vect, 0);
	check(100 == node->node.n);
	check(1 == node->next);
	node = (plnode_true *)vect_void_get(vect, 1);
	check(101 == node->node.n);
	check(2 == node->next);
	node = (plnode_true *)vect_void_get(vect, 2);
	check(102 == node->node.n);
	check(GPOOL_NO_NEXT == node->next);

	gpool_id get_0, get_1, get_2;

	get_0 = gpool_void_get(pool);
	check(3 == vect_void_cap(vect));
	check(0 == gpool_id_unpack(get_0));
	check(1 == vect_void_len(vect));
	pl_node = (plnode *)gpool_void_reach(pool, get_0);
	node = (plnode_true *)gpool_void_reach(pool, get_0);
	check(100 == pl_node->n);
	check(100 == node->node.n);
	check(1 == node->next);

	get_1 = gpool_void_get(pool);
	check(3 == vect_void_cap(vect));
	check(1 == gpool_id_unpack(get_1));
	check(2 == vect_void_len(vect));
	pl_node = (plnode *)gpool_void_reach(pool, get_1);
	node = (plnode_true *)gpool_void_reach(pool, get_1);
	check(101 == pl_node->n);
	check(101 == node->node.n);
	check(2 == node->next);

	get_2 = gpool_void_get(pool);
	check(3 == vect_void_cap(vect));
	check(2 == gpool_id_unpack(get_2));
	pl_node = (plnode *)gpool_void_reach(pool, get_2);
	node = (plnode_true *)gpool_void_reach(pool, get_2);
	check(102 == pl_node->n);
	check(102 == node->node.n);
	check(GPOOL_NO_NEXT == node->next);
	check(GPOOL_NO_NEXT == vect_void_len(vect));
	check((3+CTOR_COUNT_INIT) == ctor_count.count);

	gpool_void_put_back(pool, get_1);
	check(3 == vect_void_cap(vect));
	check(1 == vect_void_len(vect));
	check((3+CTOR_COUNT_INIT) == ctor_count.count);
	check(((plnode_true *)vect_void_get(vect, 1))->next == GPOOL_NO_NEXT);

	get_1 = gpool_void_get(pool);
	check(3 == vect_void_cap(vect));
	check(1 == gpool_id_unpack(get_1));
	pl_node = (plnode *)gpool_void_reach(pool, get_1);
	node = (plnode_true *)gpool_void_reach(pool, get_1);
	check(101 == pl_node->n);
	check(101 == node->node.n);
	check(GPOOL_NO_NEXT == node->next);
	check(GPOOL_NO_NEXT == vect_void_len(vect));
	check((3+CTOR_COUNT_INIT) == ctor_count.count);

	// grows x2 here
	ctor_count.add = 200;

	gpool_id next = gpool_void_get(pool);
	check((6+CTOR_COUNT_INIT) == ctor_count.count);
	check(3 == gpool_id_unpack(next));
	check(6 == vect_void_cap(vect));
	pl_node = (plnode *)gpool_void_reach(pool, next);
	node = (plnode_true *)gpool_void_reach(pool, next);
	check(203 == pl_node->n);
	check(203 == node->node.n);
	check(4 == node->next);
	check(4 == vect_void_len(vect));
	check((6+CTOR_COUNT_INIT) == ctor_count.count);
	
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(0)))->node.n == 100);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(0)))->next == 1);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(1)))->node.n == 101);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(1)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(2)))->node.n == 102);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(2)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(3)))->node.n == 203);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(3)))->next == 4);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(4)))->node.n == 204);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(4)))->next == 5);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(5)))->node.n == 205);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(5)))->next == GPOOL_NO_NEXT);

	check(0 == g_dtor_count);
	gpool_void_destroy(pool);
	check(6 == g_dtor_count);
	g_dtor_count = 0;
	check(0 == g_dtor_count);

	return true;
}

static bool test_gpool_void_stress(void)
{
	grow_pool_void pool_, * pool = &pool_;
	vect_void * vect = &pool->vect;

	plctor_arg ctor_count = {CTOR_COUNT_INIT, 100};

	check(gpool_void_create_sz(pool,
			sizeof(plnode), 1, ctor, &ctor_count, dtor) == pool);
	check(1 == vect_void_cap(vect));
	check(1 == ctor_count.count);

	size_t i = 0;
	size_t limit = 128;
	plnode_true * nd = NULL;
	bool chk_n, chk_next;

	check(vect_void_len(vect) == 0);
	for (i = 0; i < limit; ++i)
		gpool_void_get(pool);
	check(vect_void_len(vect) == GPOOL_NO_NEXT);

	check(vect_void_cap(vect) == limit);
	check(ctor_count.count == limit);

	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(0)))->node.n == 100);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(0)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(1)))->node.n == 101);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(1)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(3)))->node.n == 103);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(3)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(7)))->node.n == 107);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(7)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(15)))->node.n == 115);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(15)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(31)))->node.n == 131);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(31)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(63)))->node.n == 163);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(63)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(127)))->node.n == 227);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(127)))->next == GPOOL_NO_NEXT);

	for (i = 0; i < limit; ++i)
		gpool_void_put_back(pool, gpool_id_pack(i));

	check(vect_void_len(vect) == limit-1);
	check(vect_void_cap(vect) == limit);
	check(ctor_count.count == limit);

	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(0)))->node.n == 100);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(0)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(1)))->node.n == 101);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(1)))->next == 0);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(3)))->node.n == 103);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(3)))->next == 2);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(7)))->node.n == 107);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(7)))->next == 6);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(15)))->node.n == 115);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(15)))->next == 14);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(31)))->node.n == 131);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(31)))->next == 30);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(63)))->node.n == 163);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(63)))->next == 62);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(127)))->node.n == 227);
	check(((plnode_true *)gpool_void_reach(pool, gpool_id_pack(127)))->next == 126);

	for (i = limit-1; i > 0; --i)
	{
		nd = (plnode_true *)gpool_void_reach(pool, gpool_id_pack(i));
		chk_n = ((i + ctor_count.add) == nd->node.n);
		chk_next = ((i-1) == nd->next);

		if (!(chk_n && chk_next))
		{
			printf("error at loop iter: %zu, %d, %zu\n",
				i, nd->node.n, nd->next);
		}

		check(chk_n);
		check(chk_next);
	}
	nd = (plnode_true *)gpool_void_reach(pool, gpool_id_pack(0));
	chk_n = ((0 + ctor_count.add) == nd->node.n);
	chk_next = (GPOOL_NO_NEXT == nd->next);

	check(vect_void_cap(vect) == limit);
	check(ctor_count.count == limit);

	check(0 == g_dtor_count);
	gpool_void_destroy(pool);
	check(limit == g_dtor_count);
	g_dtor_count = 0;
	check(0 == g_dtor_count);

	return true;
}


typedef struct alloc_s {
	void * mem;
} alloc_s;

static void ctor_alloc(void * elem, void * arg)
{
	UNUSED(arg);
	alloc_s * pelem = (alloc_s *)elem;
	pelem->mem = malloc(1);
	if (!pelem->mem)
		exit(EXIT_FAILURE);
}

static void dtor_alloc(void * elem)
{
	UNUSED(elem);
	alloc_s * pelem = (alloc_s *)elem;
	free(pelem->mem);
}

// should have no issues when run with sanitizer/valgrind
static bool test_gpool_void_alloc(void)
{
	grow_pool_void pool_, * pool = &pool_;

	size_t i = 0;
	size_t limit = 128;

	check(gpool_void_create_sz(pool,
			sizeof(alloc_s), 1, ctor_alloc, NULL, dtor_alloc) == pool);

	for (i = 0; i < limit; ++i)
		gpool_void_get(pool);

	gpool_void_destroy(pool);


	check(gpool_void_create_sz(pool,
			sizeof(alloc_s), 1, ctor_alloc, NULL, dtor_alloc) == pool);

	for (i = 0; i < limit; ++i)
		gpool_void_get(pool);

	for (i = 0; i < limit; ++i)
		gpool_void_put_back(pool, gpool_id_pack(i));

	gpool_void_destroy(pool);


	check(gpool_void_create_sz(pool,
			sizeof(alloc_s), 1, ctor_alloc, NULL, dtor_alloc) == pool);

	for (i = 0; i < limit; ++i)
		gpool_void_get(pool);

	for (i = limit-1; i > 0; --i)
		gpool_void_put_back(pool, gpool_id_pack(i));
	gpool_void_put_back(pool, gpool_id_pack(0));

	gpool_void_destroy(pool);

	return true;
}

GROW_POOL_DEFINE(node, plnode);

static bool test_gpool_node_create(void)
{
	grow_pool_node pool_, * pool = &pool_;
	vect_void * vect = &pool->pool.vect;

	{
		check(gpool_node_create_pod(pool, 3) == pool);
		check(vect_void_data(vect));
		check(sizeof(size_t)*2 == vect_void_elem_size(vect));
		check(0 == vect_void_len(vect));
		check(3 == vect_void_cap(vect));

		check(!pool->pool.ctor);
		check(!pool->pool.ctor_arg);
		check(!pool->pool.dtor);

		plnode_true * node = NULL;
		node = (plnode_true *)vect_void_get(vect, 0);
		check(1 == node->next);
		node = (plnode_true *)vect_void_get(vect, 1);
		check(2 == node->next);
		node = (plnode_true *)vect_void_get(vect, 2);
		check(GPOOL_NO_NEXT == node->next);

		gpool_node_destroy(pool);
	}

	{
		plctor_arg ctor_count = {CTOR_COUNT_INIT, 100};

		check(gpool_node_create_sz(pool, 3, ctor, &ctor_count, dtor) == pool);
		check(vect_void_data(vect));
		check(sizeof(size_t)*2 == vect_void_elem_size(vect));
		check(0 == vect_void_len(vect));
		check(3 == vect_void_cap(vect));

		check((fvoid)ctor == pool->pool.ctor);
		check((void *)&ctor_count == pool->pool.ctor_arg);
		check((fvoid)dtor == pool->pool.dtor);

		plnode_true * node = NULL;
		node = (plnode_true *)vect_void_get(vect, 0);
		check(100 == node->node.n);
		check(1 == node->next);
		node = (plnode_true *)vect_void_get(vect, 1);
		check(101 == node->node.n);
		check(2 == node->next);
		node = (plnode_true *)vect_void_get(vect, 2);
		check(102 == node->node.n);
		check(GPOOL_NO_NEXT == node->next);

		check((3+CTOR_COUNT_INIT) == ctor_count.count);

		check(0 == g_dtor_count);
		gpool_node_destroy(pool);
		check(3 == g_dtor_count);
		g_dtor_count = 0;
		check(0 == g_dtor_count);
	}

	return true;
}

static bool test_gpool_node_use(void)
{
	grow_pool_node pool_, * pool = &pool_;
	vect_void * vect = &pool->pool.vect;

	plctor_arg ctor_count = {CTOR_COUNT_INIT, 100};

	check(gpool_node_create_sz(pool, 3, ctor, &ctor_count, dtor) == pool);
	check(vect_void_data(vect));
	check(sizeof(size_t)*2 == vect_void_elem_size(vect));
	check(0 == vect_void_len(vect));
	check(3 == vect_void_cap(vect));

	check((fvoid)ctor == pool->pool.ctor);
	check((void *)&ctor_count == pool->pool.ctor_arg);
	check((fvoid)dtor == pool->pool.dtor);

	check((3+CTOR_COUNT_INIT) == ctor_count.count);

	plnode * pl_node = NULL;
	plnode_true * node = NULL;
	node = (plnode_true *)vect_void_get(vect, 0);
	check(100 == node->node.n);
	check(1 == node->next);
	node = (plnode_true *)vect_void_get(vect, 1);
	check(101 == node->node.n);
	check(2 == node->next);
	node = (plnode_true *)vect_void_get(vect, 2);
	check(102 == node->node.n);
	check(GPOOL_NO_NEXT == node->next);

	gpool_id get_0, get_1, get_2;

	get_0 = gpool_node_get(pool);
	check(3 == vect_void_cap(vect));
	check(0 == gpool_id_unpack(get_0));
	check(1 == vect_void_len(vect));
	pl_node = gpool_node_reach(pool, get_0);
	node = (plnode_true *)gpool_node_reach(pool, get_0);
	check(100 == pl_node->n);
	check(100 == node->node.n);
	check(1 == node->next);

	get_1 = gpool_node_get(pool);
	check(3 == vect_void_cap(vect));
	check(1 == gpool_id_unpack(get_1));
	check(2 == vect_void_len(vect));
	pl_node = gpool_node_reach(pool, get_1);
	node = (plnode_true *)gpool_node_reach(pool, get_1);
	check(101 == pl_node->n);
	check(101 == node->node.n);
	check(2 == node->next);

	get_2 = gpool_node_get(pool);
	check(3 == vect_void_cap(vect));
	check(2 == gpool_id_unpack(get_2));
	pl_node = gpool_node_reach(pool, get_2);
	node = (plnode_true *)gpool_node_reach(pool, get_2);
	check(102 == pl_node->n);
	check(102 == node->node.n);
	check(GPOOL_NO_NEXT == node->next);
	check(GPOOL_NO_NEXT == vect_void_len(vect));
	check((3+CTOR_COUNT_INIT) == ctor_count.count);

	gpool_node_put_back(pool, get_1);
	check(3 == vect_void_cap(vect));
	check(1 == vect_void_len(vect));
	check((3+CTOR_COUNT_INIT) == ctor_count.count);
	check(((plnode_true *)vect_void_get(vect, 1))->next == GPOOL_NO_NEXT);

	get_1 = gpool_node_get(pool);
	check(3 == vect_void_cap(vect));
	check(1 == gpool_id_unpack(get_1));
	pl_node = gpool_node_reach(pool, get_1);
	node = (plnode_true *)gpool_node_reach(pool, get_1);
	check(101 == pl_node->n);
	check(101 == node->node.n);
	check(GPOOL_NO_NEXT == node->next);
	check(GPOOL_NO_NEXT == vect_void_len(vect));
	check((3+CTOR_COUNT_INIT) == ctor_count.count);

	// grows x2 here
	ctor_count.add = 200;

	gpool_id next = gpool_node_get(pool);
	check((6+CTOR_COUNT_INIT) == ctor_count.count);
	check(3 == gpool_id_unpack(next));
	check(6 == vect_void_cap(vect));
	pl_node = gpool_node_reach(pool, next);
	node = (plnode_true *)gpool_node_reach(pool, next);
	check(203 == pl_node->n);
	check(203 == node->node.n);
	check(4 == node->next);
	check(4 == vect_void_len(vect));
	check((6+CTOR_COUNT_INIT) == ctor_count.count);

	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->node.n == 100);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->next == 1);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->node.n == 101);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->next == GPOOL_NO_NEXT);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(2)))->node.n == 102);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(2)))->next == GPOOL_NO_NEXT);

	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->node.n == 203);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->next == 4);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(4)))->node.n == 204);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(4)))->next == 5);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(5)))->node.n == 205);
	check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(5)))->next == GPOOL_NO_NEXT);

	check(0 == g_dtor_count);
	gpool_node_destroy(pool);
	check(6 == g_dtor_count);
	g_dtor_count = 0;
	check(0 == g_dtor_count);

	return true;
}

static bool test_gpool_node_stress(void)
{
	grow_pool_node pool_, * pool = &pool_;
	vect_void * vect = &pool->pool.vect;

	{
		plctor_arg ctor_count = {CTOR_COUNT_INIT, 100};

		check(gpool_node_create_sz(pool, 1, ctor, &ctor_count, dtor) == pool);
		check(1 == vect_void_cap(vect));
		check(1 == ctor_count.count);

		size_t i = 0;
		size_t limit = 128;
		plnode_true * nd = NULL;
		bool chk_n, chk_next;

		check(vect_void_len(vect) == 0);
		for (i = 0; i < limit; ++i)
			gpool_node_get(pool);
		check(vect_void_len(vect) == GPOOL_NO_NEXT);

		check(vect_void_cap(vect) == limit);
		check(ctor_count.count == limit);

		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->node.n == 100);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->node.n == 101);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->node.n == 103);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(7)))->node.n == 107);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(7)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(15)))->node.n == 115);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(15)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(31)))->node.n == 131);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(31)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(63)))->node.n == 163);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(63)))->next ==
			GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(127)))->node.n == 227);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(127)))->next
			== GPOOL_NO_NEXT);

		for (i = 0; i < limit; ++i)
			gpool_node_put_back(pool, gpool_id_pack(i));

		check(vect_void_len(vect) == limit-1);
		check(vect_void_cap(vect) == limit);
		check(ctor_count.count == limit);

		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->node.n == 100);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->node.n == 101);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->next == 0);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->node.n == 103);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->next == 2);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(7)))->node.n == 107);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(7)))->next == 6);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(15)))->node.n == 115);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(15)))->next == 14);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(31)))->node.n == 131);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(31)))->next == 30);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(63)))->node.n == 163);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(63)))->next == 62);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(127)))->node.n == 227);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(127)))->next == 126);

		for (i = limit-1; i > 0; --i)
		{
			nd = (plnode_true *)gpool_node_reach(pool, gpool_id_pack(i));
			chk_n = ((i + ctor_count.add) == nd->node.n);
			chk_next = ((i-1) == nd->next);

			if (!(chk_n && chk_next))
			{
				printf("error at loop iter: %zu, %d, %zu\n",
					i, nd->node.n, nd->next);
			}

			check(chk_n);
			check(chk_next);
		}
		nd = (plnode_true *)gpool_node_reach(pool, gpool_id_pack(0));
		check((0 + ctor_count.add) == nd->node.n);
		check(GPOOL_NO_NEXT == nd->next);

		check(vect_void_cap(vect) == limit);
		check(ctor_count.count == limit);

		check(0 == g_dtor_count);
		gpool_node_destroy(pool);
		check(limit == g_dtor_count);
		g_dtor_count = 0;
		check(0 == g_dtor_count);
	}

	{
		plctor_arg ctor_count = {CTOR_COUNT_INIT, 100};

		check(gpool_node_create_sz(pool, 1, ctor, &ctor_count, dtor) == pool);
		check(1 == vect_void_cap(vect));
		check(1 == ctor_count.count);

		size_t i = 0;
		size_t limit = 128;
		plnode_true * nd = NULL;
		bool chk_n, chk_next;

		check(vect_void_len(vect) == 0);
		for (i = 0; i < limit; ++i)
			gpool_node_get(pool);
		check(vect_void_len(vect) == GPOOL_NO_NEXT);

		check(vect_void_cap(vect) == limit);
		check(ctor_count.count == limit);

		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->node.n == 100);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->node.n == 101);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->node.n == 103);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(7)))->node.n == 107);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(7)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(15)))->node.n == 115);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(15)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(31)))->node.n == 131);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(31)))->next
			== GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(63)))->node.n == 163);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(63)))->next ==
			GPOOL_NO_NEXT);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(127)))->node.n == 227);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(127)))->next
			== GPOOL_NO_NEXT);

		for (i = limit-1; i > 0; --i)
			gpool_node_put_back(pool, gpool_id_pack(i));
		gpool_node_put_back(pool, gpool_id_pack(0));

		check(vect_void_len(vect) == 0);
		check(vect_void_cap(vect) == limit);
		check(ctor_count.count == limit);

		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->node.n == 100);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(0)))->next == 1);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->node.n == 101);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(1)))->next == 2);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->node.n == 103);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(3)))->next == 4);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(7)))->node.n == 107);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(7)))->next == 8);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(15)))->node.n == 115);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(15)))->next == 16);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(31)))->node.n == 131);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(31)))->next == 32);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(63)))->node.n == 163);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(63)))->next == 64);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(127)))->node.n == 227);
		check(((plnode_true *)gpool_node_reach(pool, gpool_id_pack(127)))->next
			== GPOOL_NO_NEXT);

		size_t last = limit-1;
		for (i = 0; i < last; ++i)
		{
			nd = (plnode_true *)gpool_node_reach(pool, gpool_id_pack(i));
			chk_n = ((i + ctor_count.add) == nd->node.n);
			chk_next = ((i+1) == nd->next);

			if (!(chk_n && chk_next))
			{
				printf("error at loop iter: %zu, %d, %zu\n",
					i, nd->node.n, nd->next);
			}

			check(chk_n);
			check(chk_next);
		}
		nd = (plnode_true *)gpool_node_reach(pool, gpool_id_pack(last));
		check((last + ctor_count.add) == nd->node.n);
		check(GPOOL_NO_NEXT == nd->next);

		check(vect_void_cap(vect) == limit);
		check(ctor_count.count == limit);

		check(0 == g_dtor_count);
		gpool_node_destroy(pool);
		check(limit == g_dtor_count);
		g_dtor_count = 0;
		check(0 == g_dtor_count);
	}

	return true;
}

GROW_POOL_DEFINE(allocs, alloc_s);

static bool test_gpool_allocs_alloc(void)
{
	grow_pool_allocs pool_, * pool = &pool_;

	size_t i = 0;
	size_t limit = 128;

	check(gpool_allocs_create_sz(pool,
		1, ctor_alloc, NULL, dtor_alloc) == pool);

	for (i = 0; i < limit; ++i)
		gpool_allocs_get(pool);

	gpool_allocs_destroy(pool);


	check(gpool_allocs_create_sz(pool,
		1, ctor_alloc, NULL, dtor_alloc) == pool);

	for (i = 0; i < limit; ++i)
		gpool_allocs_get(pool);

	for (i = 0; i < limit; ++i)
		gpool_allocs_put_back(pool, gpool_id_pack(i));

	gpool_allocs_destroy(pool);


	check(gpool_allocs_create_sz(pool,
			1, ctor_alloc, NULL, dtor_alloc) == pool);

	for (i = 0; i < limit; ++i)
		gpool_allocs_get(pool);

	for (i = limit-1; i > 0; --i)
		gpool_allocs_put_back(pool, gpool_id_pack(i));
	gpool_allocs_put_back(pool, gpool_id_pack(0));

	gpool_allocs_destroy(pool);

	return true;
}

static ftest tests[] = {
	test_gpool_void_create,
	test_gpool_void_use,
	test_gpool_void_stress,
	test_gpool_void_alloc,
	test_gpool_node_create,
	test_gpool_node_use,
	test_gpool_node_stress,
	test_gpool_allocs_alloc,
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
