#include <stdio.h>
#include "../tests/test.h"
#include "nslist/nslist.h"

typedef struct int_node {
	int n;
	struct int_node * next;
} int_node;

typedef struct acc {
	int n;
	int m;
} acc;

static bool apply_void_node_a(void * node, void * arg)
{
	int_node * pnode = (int_node *)node;
	acc * pacc = (acc *)arg;
	++pacc->m;
	pacc->n += pnode->n;
	return true;
}

static bool apply_void_node_b(void * node, void * arg)
{
	int_node * pnode = (int_node *)node;
	acc * pacc = (acc *)arg;
	
	if (++pacc->m == 2)
	{
		pacc->n += pnode->n;
		return false;
	}
	return true;
}

static char x = 0;
static void * not_null = &x;

static bool nslist_test(void)
{
	nslist_void list_, * list = &list_;

	nslist_void_init(list, offsetof(int_node, next));
	check(NULL == list->head);

	nslist_void_set(list, not_null);
	check(not_null = list->head);
	nslist_void_set(list, NULL);
	check(NULL == list->head);
	
	static int_node dummy;
	int_node * not_null = &dummy;
	
	int_node a = {111, not_null};
	int_node b = {222, not_null};
	int_node c = {333, not_null};
	
	nslist_void_push(list, &a);
	check(&a == nslist_void_head(list));
	check(&a == list->head);
	check(NULL == a.next);
	
	nslist_void_push(list, &b);
	check(&b == nslist_void_head(list));
	check(&b == list->head);
	check(&a == b.next);
	
	nslist_void_push(list, &c);
	check(&c == nslist_void_head(list));
	check(&c == list->head);
	check(&b == c.next);

	check(333 == ((int_node *)nslist_void_head(list))->n);
	check(222 == ((int_node *)nslist_void_next(list,
		nslist_void_head(list)))->n);
	check(111 == ((int_node *)nslist_void_next(list,
		nslist_void_next(list,
			nslist_void_head(list))))->n);
	check(NULL == ((int_node *)nslist_void_next(list,
		nslist_void_next(list,
			nslist_void_next(list,
				nslist_void_head(list))))));

	check(333 == ((int_node *)nslist_void_head(list))->n);
	check(222 == (*(int_node **)nslist_void_pnext(list,
		nslist_void_head(list)))->n);
	check(111 == (*(int_node **)nslist_void_pnext(list,
		nslist_void_next(list,
			nslist_void_head(list))))->n);
	check(NULL == (*(int_node **)nslist_void_pnext(list,
		nslist_void_next(list,
			nslist_void_next(list,
				nslist_void_head(list))))));
		
	acc sum = {0, 0};
	check(0 == sum.n);
	check(0 == sum.m);
	
	nslist_void_apply(list, apply_void_node_a, &sum);
	check(666 == sum.n);
	check(3 == sum.m);
	
	sum.n = 0;
	sum.m = 0;
	
	check(0 == sum.n);
	check(0 == sum.m);
	nslist_void_apply(list, apply_void_node_b, &sum);
	check(222 == sum.n);
	check(2 == sum.m);
	
	int_node * node = (int_node *)nslist_void_pop(list);
	check(node == &c);
	check(node->n == c.n);
	
	node = (int_node *)nslist_void_pop(list);
	check(node == &b);
	check(node->n == b.n);
	
	node = (int_node *)nslist_void_pop(list);
	check(node == &a);
	check(node->n == a.n);
	
	node = (int_node *)nslist_void_pop(list);
	check(NULL == node);
	
	node = (int_node *)nslist_void_pop(list);
	check(NULL == node);
	
	nslist_void_init(list, offsetof(int_node, next));
	check(NULL == list->head);
	
	return true;
}

NSLIST_DEFINE(intn, int_node);

static bool apply_node_a(int_node * node, void * arg)
{
	acc * pacc = (acc *)arg;
	++pacc->m;
	pacc->n += node->n;
	return true;
}

static bool apply_node_b(int_node * node, void * arg)
{
	acc * pacc = (acc *)arg;
	
	if (++pacc->m == 2)
	{
		pacc->n += node->n;
		return false;
	}
	return true;
}

static bool nslist_typed_test(void)
{
	nslist_intn list_, * list = &list_;

	nslist_intn_init(list);
	check(NULL == list->head);

	nslist_intn_set(list, (int_node *)not_null);
	check(not_null == list->head);
	nslist_intn_set(list, NULL);
	check(NULL == list->head);
	
	static int_node dummy;
	int_node * not_null = &dummy;
	
	int_node a = {111, not_null};
	int_node b = {222, not_null};
	int_node c = {333, not_null};
	
	nslist_intn_push(list, &a);
	check(&a == nslist_intn_head(list));
	check(&a == list->head);
	check(NULL == a.next);
	
	nslist_intn_push(list, &b);
	check(&b == nslist_intn_head(list));
	check(&b == list->head);
	check(&a == b.next);
	
	nslist_intn_push(list, &c);
	check(&c == nslist_intn_head(list));
	check(&c == list->head);
	check(&b == c.next);
	
	acc sum = {0, 0};
	check(0 == sum.n);
	check(0 == sum.m);
	
	nslist_intn_apply(list, apply_node_a, &sum);
	check(666 == sum.n);
	check(3 == sum.m);
	
	sum.n = 0;
	sum.m = 0;
	
	check(0 == sum.n);
	check(0 == sum.m);
	nslist_intn_apply(list, apply_node_b, &sum);
	check(222 == sum.n);
	check(2 == sum.m);
	
	int_node * node = nslist_intn_pop(list);
	check(node == &c);
	check(node->n == c.n);
	
	node = nslist_intn_pop(list);
	check(node == &b);
	check(node->n == b.n);
	
	node = nslist_intn_pop(list);
	check(node == &a);
	check(node->n == a.n);
	
	node = nslist_intn_pop(list);
	check(NULL == node);
	
	node = nslist_intn_pop(list);
	check(NULL == node);
	
	nslist_intn_init(list);
	check(NULL == list->head);
	
	return true;
}

static ftest tests[] = {
	nslist_test,
	nslist_typed_test,
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
