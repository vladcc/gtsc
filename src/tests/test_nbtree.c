#include <stdio.h>
#include "test.h"

#include <string.h>
#include "nbtree/nbtree.h"

typedef struct tnode {
	char data;
	struct tnode * left;
	struct tnode * right;
} tnode;

static tnode * make_tree(void)
{
	// 1 + 2 * 5

	//     +
	//  1     *
	//      2   5
	
	static tnode tnodes[5];

	tnode * n1 = tnodes+0;
	tnode * np = tnodes+1;
	tnode * n2 = tnodes+2;
	tnode * nm = tnodes+3;	
	tnode * n5 = tnodes+4;

	np->data = '+';
	np->left = n1;
	np->right = nm;

	nm->data = '*';
	nm->left = n2;
	nm->right = n5;

	n1->data = '1';
	n1->left = n1->right = NULL;

	n2->data = '2';
	n2->left = n2->right = NULL;

	n5->data = '5';
	n5->left = n5->right = NULL;
	
	return np;
}

typedef struct test_str {
	char str[6];
	int len;
} test_str;

static void visit(void * nod, void * arg)
{
	tnode * pnode = (tnode *)nod;
	test_str * str = (test_str *)arg;
	str->str[str->len++] = pnode->data;
}

static bool test_nbtree_void(void)
{
	{
		tnode node_, * node = &node_;
		node->left = NULL;
		node->right = NULL;
		
		nbtree_void tree_, * tree = &tree_;
		nbtree_void_init(tree, offsetof(tnode, left), offsetof(tnode, right));
		nbtree_void_set_root(tree, node);

		void * root = nbtree_void_get_root(tree);
		check(root == node);
		
		check(nbtree_void_get_left(tree, root) == NULL);
		check(nbtree_void_get_right(tree, root) == NULL);

		check(nbtree_void_get_pleft(tree, root) == (void **)&node->left);
		check(nbtree_void_get_pright(tree, root) == (void **)&node->right);

		static char ch = 0;
		void * not_null = &ch;

		nbtree_void_set_left(tree, root, not_null);
		check(nbtree_void_get_left(tree, root) == not_null);
		check(nbtree_void_get_right(tree, root) == NULL);

		nbtree_void_set_right(tree, root, not_null);
		check(nbtree_void_get_left(tree, root) == not_null);
		check(nbtree_void_get_right(tree, root) == not_null);

		nbtree_void_make_leaf(tree, root);
		check(nbtree_void_get_left(tree, root) == NULL);
		check(nbtree_void_get_right(tree, root) == NULL);
	}
	
	tnode * root = make_tree();

	nbtree_void tree_, * tree = &tree_;
	nbtree_void_init(tree, offsetof(tnode, left), offsetof(tnode, right));
	nbtree_void_set_root(tree, root);

	check(nbtree_void_get_root(tree) == root);

	static const char pre_nlr[] = "+1*25";
	static const char post_lrn[] = "125*+";
	static const char in_lnr[] = "1+2*5";

	static const char rpre_nrl[] = "+*521";
	static const char rpost_rln[] = "52*1+";
	static const char rin_rnl[] = "5*2+1";

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));
		
		nbtree_void tree_null_, * tree_null = &tree_null_;
		nbtree_void_set_root(tree_null, NULL);
		nbtree_void_pre_nlr(tree_null, NULL, NULL);
		
		nbtree_void_pre_nlr(tree, visit, str);
		check(strcmp(str->str, pre_nlr) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));
		
		nbtree_void tree_null_, * tree_null = &tree_null_;
		nbtree_void_set_root(tree_null, NULL);
		nbtree_void_post_lrn(tree_null, NULL, NULL);

		nbtree_void_post_lrn(tree, visit, str);
		check(strcmp(str->str, post_lrn) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));
		
		nbtree_void tree_null_, * tree_null = &tree_null_;
		nbtree_void_set_root(tree_null, NULL);
		nbtree_void_in_lnr(tree_null, NULL, NULL);
		
		nbtree_void_in_lnr(tree, visit, str);
		check(strcmp(str->str, in_lnr) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));

		nbtree_void tree_null_, * tree_null = &tree_null_;
		nbtree_void_set_root(tree_null, NULL);
		nbtree_void_rpre_nrl(tree_null, NULL, NULL);

		nbtree_void_rpre_nrl(tree, visit, str);
		check(strcmp(str->str, rpre_nrl) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));
		
		nbtree_void tree_null_, * tree_null = &tree_null_;
		nbtree_void_set_root(tree_null, NULL);
		nbtree_void_rpost_rln(tree_null, NULL, NULL);
		
		nbtree_void_rpost_rln(tree, visit, str);
		check(strcmp(str->str, rpost_rln) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));

		nbtree_void tree_null_, * tree_null = &tree_null_;
		nbtree_void_set_root(tree_null, NULL);
		nbtree_void_rin_rnl(tree_null, NULL, NULL);

		nbtree_void_rin_rnl(tree, visit, str);
		check(strcmp(str->str, rin_rnl) == 0);
	}
	
	return true;
}

NBTREE_DEFINE(nodet, tnode);

static bool test_nbtree_nodet(void)
{
	{
		tnode node_, * node = &node_;
		node->left = NULL;
		node->right = NULL;
		
		nbtree_nodet tree_, * tree = &tree_;
		nbtree_nodet_init(tree);
		nbtree_nodet_set_root(tree, node);

		tnode * root = nbtree_nodet_get_root(tree);
		check(root == node);
		
		check(nbtree_nodet_get_left(tree, root) == NULL);
		check(nbtree_nodet_get_right(tree, root) == NULL);

		static char ch = 0;
		void * not_null = &ch;

		nbtree_nodet_set_left(tree, root, (tnode *)not_null);
		check(nbtree_nodet_get_left(tree, root) == not_null);
		check(nbtree_nodet_get_right(tree, root) == NULL);

		nbtree_nodet_set_right(tree, root, (tnode *)not_null);
		check(nbtree_nodet_get_left(tree, root) == not_null);
		check(nbtree_nodet_get_right(tree, root) == not_null);

		nbtree_nodet_make_leaf(tree, root);
		check(nbtree_nodet_get_left(tree, root) == NULL);
		check(nbtree_nodet_get_right(tree, root) == NULL);
	}
	
	tnode * root = make_tree();

	nbtree_nodet tree_, * tree = &tree_;
	nbtree_nodet_init(tree);
	nbtree_nodet_set_root(tree, root);

	check(nbtree_nodet_get_root(tree) == root);

	static const char pre_nlr[] = "+1*25";
	static const char post_lrn[] = "125*+";
	static const char in_lnr[] = "1+2*5";

	static const char rpre_nrl[] = "+*521";
	static const char rpost_rln[] = "52*1+";
	static const char rin_rnl[] = "5*2+1";

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));

		nbtree_nodet tree_null_, * tree_null = &tree_null_;
		nbtree_nodet_set_root(tree_null, NULL);
		nbtree_nodet_pre_nlr(tree_null, NULL, NULL);

		nbtree_nodet_pre_nlr(tree, visit, str);
		check(strcmp(str->str, pre_nlr) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));

		nbtree_nodet tree_null_, * tree_null = &tree_null_;
		nbtree_nodet_set_root(tree_null, NULL);
		nbtree_nodet_post_lrn(tree_null, NULL, NULL);

		nbtree_nodet_post_lrn(tree, visit, str);
		check(strcmp(str->str, post_lrn) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));

		nbtree_nodet tree_null_, * tree_null = &tree_null_;
		nbtree_nodet_set_root(tree_null, NULL);
		nbtree_nodet_in_lnr(tree_null, NULL, NULL);

		nbtree_nodet_in_lnr(tree, visit, str);
		check(strcmp(str->str, in_lnr) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));

		nbtree_nodet tree_null_, * tree_null = &tree_null_;
		nbtree_nodet_set_root(tree_null, NULL);
		nbtree_nodet_rpre_nrl(tree_null, NULL, NULL);

		nbtree_nodet_rpre_nrl(tree, visit, str);
		check(strcmp(str->str, rpre_nrl) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));

		nbtree_nodet tree_null_, * tree_null = &tree_null_;
		nbtree_nodet_set_root(tree_null, NULL);
		nbtree_nodet_rpost_rln(tree_null, NULL, NULL);

		nbtree_nodet_rpost_rln(tree, visit, str);
		check(strcmp(str->str, rpost_rln) == 0);
	}

	{
		test_str str_, * str = &str_;
		memset(str, 0, sizeof(*str));

		nbtree_nodet tree_null_, * tree_null = &tree_null_;
		nbtree_nodet_set_root(tree_null, NULL);
		nbtree_nodet_rin_rnl(tree_null, NULL, NULL);

		nbtree_nodet_rin_rnl(tree, visit, str);
		check(strcmp(str->str, rin_rnl) == 0);
	}
	
	return true;
}

static ftest tests[] = {
	test_nbtree_void,
	test_nbtree_nodet,
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
