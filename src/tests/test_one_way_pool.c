#include <stdio.h>
#include "test.h"
#include "one_way_pool/one_way_pool.h"

static bool test_owp_void(void)
{
	owp_void pool_, * pool = &pool_;

	{
		typedef struct node {
			char ch[1];
		} node;

		nslist_void * pool_list;
		
		check(owp_void_create(pool, sizeof(node), 3) == pool);
		pool_list = &pool->pool_list;
		check((pool_list->next_offs % sizeof(void *)) == 0);
		check(sizeof(void *) == pool_list->next_offs);

		node * n = NULL;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 1;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 2;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 3;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 4;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 5;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 6;

		node * chk = NULL;
		chk = (node *)nslist_void_head(pool_list);
		check(4 == chk->ch[0]);
		++chk;
		check(5 == chk->ch[0]);
		++chk;
		check(6 == chk->ch[0]);

		chk = (node *)*nslist_void_pnext(pool_list, nslist_void_head(pool_list));
		check(1 == chk->ch[0]);
		++chk;
		check(2 == chk->ch[0]);
		++chk;
		check(3 == chk->ch[0]);

		check(NULL == *nslist_void_pnext(pool_list,
			*nslist_void_pnext(pool_list,
				nslist_void_head(pool_list))));
		
		owp_void_destroy(pool);
	}

	{
		typedef struct node {
			char ch[sizeof(void *)-(sizeof(void *)/2)];
		} node;

		nslist_void * pool_list;
		
		check(owp_void_create(pool, sizeof(node), 3) == pool);
		pool_list = &pool->pool_list;
		check((pool_list->next_offs % sizeof(void *)) == 0);
		check(2*sizeof(void *) == pool_list->next_offs);

		node * n = NULL;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 1;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 2;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 3;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 4;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 5;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 6;

		node * chk = NULL;
		chk = (node *)nslist_void_head(pool_list);
		check(4 == chk->ch[0]);
		++chk;
		check(5 == chk->ch[0]);
		++chk;
		check(6 == chk->ch[0]);

		chk = (node *)*nslist_void_pnext(pool_list, nslist_void_head(pool_list));
		check(1 == chk->ch[0]);
		++chk;
		check(2 == chk->ch[0]);
		++chk;
		check(3 == chk->ch[0]);

		check(NULL == *nslist_void_pnext(pool_list,
			*nslist_void_pnext(pool_list,
				nslist_void_head(pool_list))));
		
		owp_void_destroy(pool);
	}
	
	{
		typedef struct node {
			char ch[sizeof(void *)-1];
		} node;

		nslist_void * pool_list;
		
		check(owp_void_create(pool, sizeof(node), 3) == pool);
		pool_list = &pool->pool_list;
		check((pool_list->next_offs % sizeof(void *)) == 0);
		check(3*sizeof(void *) == pool_list->next_offs);

		node * n = NULL;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 1;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 2;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 3;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 4;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 5;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 6;

		node * chk = NULL;
		chk = (node *)nslist_void_head(pool_list);
		check(4 == chk->ch[0]);
		++chk;
		check(5 == chk->ch[0]);
		++chk;
		check(6 == chk->ch[0]);

		chk = (node *)*nslist_void_pnext(pool_list, nslist_void_head(pool_list));
		check(1 == chk->ch[0]);
		++chk;
		check(2 == chk->ch[0]);
		++chk;
		check(3 == chk->ch[0]);

		check(NULL == *nslist_void_pnext(pool_list,
			*nslist_void_pnext(pool_list,
				nslist_void_head(pool_list))));

		owp_void_destroy(pool);
	}

	{
		typedef struct node {
			char ch[sizeof(void *)];
		} node;

		nslist_void * pool_list;
		
		check(owp_void_create(pool, sizeof(node), 3) == pool);
		pool_list = &pool->pool_list;
		check((pool_list->next_offs % sizeof(void *)) == 0);
		check(3*sizeof(void *) == pool_list->next_offs);

		node * n = NULL;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 1;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 2;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 3;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 4;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 5;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 6;

		node * chk = NULL;
		chk = (node *)nslist_void_head(pool_list);
		check(4 == chk->ch[0]);
		++chk;
		check(5 == chk->ch[0]);
		++chk;
		check(6 == chk->ch[0]);

		chk = (node *)*nslist_void_pnext(pool_list, nslist_void_head(pool_list));
		check(1 == chk->ch[0]);
		++chk;
		check(2 == chk->ch[0]);
		++chk;
		check(3 == chk->ch[0]);

		check(NULL == *nslist_void_pnext(pool_list,
			*nslist_void_pnext(pool_list,
				nslist_void_head(pool_list))));

		owp_void_destroy(pool);
	}

	{
		typedef struct node {
			char ch[sizeof(void *)+1];
		} node;

		nslist_void * pool_list;
		
		check(owp_void_create(pool, sizeof(node), 3) == pool);
		pool_list = &pool->pool_list;
		check((pool_list->next_offs % sizeof(void *)) == 0);
		check(4*sizeof(void *) == pool_list->next_offs);

		node * n = NULL;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 1;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 2;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 3;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 4;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 5;
		n = (node *)owp_void_get(pool);
		n->ch[0] = 6;

		node * chk = NULL;
		chk = (node *)nslist_void_head(pool_list);
		check(4 == chk->ch[0]);
		++chk;
		check(5 == chk->ch[0]);
		++chk;
		check(6 == chk->ch[0]);

		chk = (node *)*nslist_void_pnext(pool_list,
			nslist_void_head(pool_list));
		check(1 == chk->ch[0]);
		++chk;
		check(2 == chk->ch[0]);
		++chk;
		check(3 == chk->ch[0]);

		check(NULL == *nslist_void_pnext(pool_list,
			*nslist_void_pnext(pool_list,
				nslist_void_head(pool_list))));

		owp_void_destroy(pool);
	}
	
	return true;
}

typedef struct node {
	char ch[5];
} node;

OWP_DEFINE(tnode, node);

static bool test_owp_tnode(void)
{
	owp_tnode pool_, * pool = &pool_;

	check(owp_tnode_create(pool, 3) == pool);

	nslist_void * pool_list;
	pool_list = &pool->pool.pool_list;

	node * n = NULL;
	n = owp_tnode_get(pool);
	n->ch[0] = 1;
	n = owp_tnode_get(pool);
	n->ch[0] = 2;
	n = owp_tnode_get(pool);
	n->ch[0] = 3;
	n = owp_tnode_get(pool);
	n->ch[0] = 4;
	n = owp_tnode_get(pool);
	n->ch[0] = 5;
	n = owp_tnode_get(pool);
	n->ch[0] = 6;

	node * chk = NULL;
	chk = (node *)nslist_void_head(pool_list);
	check(4 == chk->ch[0]);
	++chk;
	check(5 == chk->ch[0]);
	++chk;
	check(6 == chk->ch[0]);

	chk = (node *)*nslist_void_pnext(pool_list, nslist_void_head(pool_list));
	check(1 == chk->ch[0]);
	++chk;
	check(2 == chk->ch[0]);
	++chk;
	check(3 == chk->ch[0]);

	check(NULL == *nslist_void_pnext(pool_list,
		*nslist_void_pnext(pool_list,
			nslist_void_head(pool_list))));

	owp_tnode_destroy(pool);
	
	return true;
}

static ftest tests[] = {
	test_owp_void,
	test_owp_tnode
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
