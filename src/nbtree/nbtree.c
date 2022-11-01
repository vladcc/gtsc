#include "nbtree.h"

static void pre_nlr(
	nbtree_void * tree,
	void * root,
	nbtree_fapply fn,
	void * arg
)
{
	void * left = nbtree_void_get_left(tree, root);
	void * right = nbtree_void_get_right(tree, root);
	
	fn(root, arg);
	
	if (left)
		pre_nlr(tree, left, fn, arg);
		
	if (right)
		pre_nlr(tree, right, fn, arg);
}

void nbtree_void_pre_nlr(nbtree_void * tree, nbtree_fapply fn, void * arg)
{
	void * root = nbtree_void_get_root(tree);
	if (root)
		pre_nlr(tree, root, fn, arg);
}

static void post_lrn(
	nbtree_void * tree,
	void * root,
	nbtree_fapply fn,
	void * arg
) 
{
	void * left = nbtree_void_get_left(tree, root);
	void * right = nbtree_void_get_right(tree, root);

	if (left)
		post_lrn(tree, left, fn, arg);
		
	if (right)
		post_lrn(tree, right, fn, arg);

	fn(root, arg);
}

void nbtree_void_post_lrn(nbtree_void * tree, nbtree_fapply fn, void * arg)
{
	void * root = nbtree_void_get_root(tree);
	if (root)
		post_lrn(tree, root, fn, arg);
}

static void in_lnr(
	nbtree_void * tree,
	void * root,
	nbtree_fapply fn,
	void * arg
)
{
	void * left = nbtree_void_get_left(tree, root);
	void * right = nbtree_void_get_right(tree, root);
	
	if (left)
		in_lnr(tree, left, fn, arg);

	fn(root, arg);
	
	if (right)
		in_lnr(tree, right, fn, arg);
}

void nbtree_void_in_lnr(nbtree_void * tree, nbtree_fapply fn, void * arg)
{
	void * root = nbtree_void_get_root(tree); 
	if (root)
		in_lnr(tree, root, fn, arg);
}

static void rpre_nrl(
	nbtree_void * tree,
	void * root,
	nbtree_fapply fn,
	void * arg
)
{
	void * right = nbtree_void_get_right(tree, root);
	void * left = nbtree_void_get_left(tree, root);

	fn(root, arg);
			
	if (right)
		rpre_nrl(tree, right, fn, arg);
	
	if (left)
		rpre_nrl(tree, left, fn, arg);
}

void nbtree_void_rpre_nrl(nbtree_void * tree, nbtree_fapply fn, void * arg)
{
	void * root = nbtree_void_get_root(tree);
	if (root)
		rpre_nrl(tree, root, fn, arg);
}

static void rpost_rln(
	nbtree_void * tree,
	void * root,
	nbtree_fapply fn,
	void * arg
)
{
	void * right = nbtree_void_get_right(tree, root);
	void * left = nbtree_void_get_left(tree, root);

	if (right)
		rpost_rln(tree, right, fn, arg);
	
	if (left)
		rpost_rln(tree, left, fn, arg);

	fn(root, arg);
}

void nbtree_void_rpost_rln(nbtree_void * tree, nbtree_fapply fn, void * arg)
{
	void * root = nbtree_void_get_root(tree);
	if (root)
		rpost_rln(tree, root, fn, arg);
}

static void rin_rnl(
	nbtree_void * tree,
	void * root,
	nbtree_fapply fn,
	void * arg
)
{
	void * right = nbtree_void_get_right(tree, root);
	void * left = nbtree_void_get_left(tree, root);
	
	if (right)
		rin_rnl(tree, right, fn, arg);

	fn(root, arg);
	
	if (left)
		rin_rnl(tree, left, fn, arg);
}

void nbtree_void_rin_rnl(nbtree_void * tree, nbtree_fapply fn, void * arg)
{
	void * root = nbtree_void_get_root(tree);
	if (root)
		rin_rnl(tree, root, fn, arg);
}
