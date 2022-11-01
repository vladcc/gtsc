#include "arr_algo.h"

#include <string.h>
#include <stdlib.h>

#define decls()                \
void * arr = inf->data;        \
size_t esz = inf->elem_size;   \
size_t start = inf->start;     \
size_t end = inf->end_non_incl

#define offset(data, index, elem_size) \
(((unsigned char *)data) + (index) * (elem_size))

#define ecount(start, end_non_incl) \
((end_non_incl) - (start))

#define byte_num(elem_count, elem_size) \
((elem_count) * (elem_size))

void arr_void_set_to(arr_void_info * inf, const void * key)
{
	decls();
	unsigned char * first = offset(arr, start, esz);
	unsigned char * pend = offset(arr, end, esz);
	
	while (first < pend)
	{
		memcpy(first, key, esz);
		first += esz;
	}
}

void arr_void_copy(arr_void_info * inf, void * dest)
{
	decls();
	memcpy(dest,
		offset(arr, start, esz),
		byte_num(ecount(start, end), esz)
	);
}

void arr_void_move(arr_void_info * inf, void * dest)
{
	decls();
	memmove(dest,
		offset(arr, start, esz),
		byte_num(ecount(start, end), esz)
	);
}

bool arr_void_is_sorted(arr_void_info * inf, arr_fcomp cmp)
{
	decls();
	unsigned char * prev = offset(arr, start, esz);
	unsigned char * curr = prev + esz;
	unsigned char * pend = offset(arr, end, esz);

	while (curr < pend)
	{
		if (cmp(prev, curr) > 0)
			return false;
		prev = curr;
		curr += esz;
	}
	
	return true;
}

#define decls_x(X)              \
void * X##_arr = X->data;       \
size_t X##_esz = X->elem_size;  \
size_t X##_start = X->start;    \
size_t X##_end = X->end_non_incl

bool arr_void_is_eq(arr_void_info * a, arr_void_info * b, arr_fcomp cmp)
{
	decls_x(a);
	decls_x(b);
	
	if (a_esz == b_esz && (a_end - a_start) == (b_end - b_start))
	{
		unsigned char * pa = offset(a_arr, a_start, a_esz);
		unsigned char * pb = offset(b_arr, b_start, b_esz);
		
		for (size_t i = 0, end = (a_end - a_start); i < end; ++i)
		{
			if (cmp(pa, pb) != 0)
				return false;
				
			pa += a_esz;
			pb += b_esz;
		}
		
		return true;
	}
	
	return false;
}

void arr_void_sort(arr_void_info * inf, arr_fcomp cmp)
{
	if (!arr_void_is_sorted(inf, cmp))
	{
		decls();
		qsort(offset(arr, start, esz), ecount(start, end), esz, cmp);
	}
}

void * arr_void_find(arr_void_info * inf, const void * key, arr_fcomp cmp)
{
	decls();
	unsigned char * curr = offset(arr, start, esz);
	unsigned char * pend = offset(arr, end, esz);

	while (curr < pend)
	{
		if (cmp(curr, key) == 0)
			return curr;
		curr += esz;
	}
	
	return NULL;
}

void * arr_void_bsearch(arr_void_info * inf, const void * key, arr_fcomp cmp)
{
	decls();
	return bsearch(key, offset(arr, start, esz), ecount(start, end), esz, cmp);
}

static void * arr_void_bound(
	arr_void_info * inf,
	const void * key,
	arr_fcomp cmp,
	bool lower
)
{
#define bound_pred() ((lower && res_cmp >= 0) || (!lower && res_cmp > 0))

	decls();
	void * ret = NULL;
	
	if (start < end)
	{
		size_t mid;
		int res_cmp;
		
		--end;
		while (start < end)
		{
			mid = start + (end - start) / 2;
			res_cmp = cmp(offset(arr, mid, esz), key);
			if (bound_pred())
				end = mid;
			else
				start = mid+1;
		}
		
		void * candidate = offset(arr, start, esz);
		res_cmp = cmp(candidate, key);

		if (bound_pred())
			ret = candidate;
	}

	return ret;
	
#undef bound_pred
}

void * arr_void_sfirst_eq(arr_void_info * inf, const void * key, arr_fcomp cmp)
{
	void * res = arr_void_sfirst_geq(inf, key, cmp);
	if (!(res && cmp(res, key) == 0))
		res = NULL;
	return res;
}

void * arr_void_sfirst_geq(arr_void_info * inf, const void * key, arr_fcomp cmp)
{
	return arr_void_bound(inf, key, cmp, true);
}

void * arr_void_sfirst_gt(arr_void_info * inf, const void * key, arr_fcomp cmp)
{
	return arr_void_bound(inf, key, cmp, false);
}

void arr_void_swap(arr_void_info * inf, size_t a, size_t b, void * elem_tmp)
{
	void * arr = inf->data;
	size_t esz = inf->elem_size;
	
	void * pa = offset(arr, a, esz);
	void * pb = offset(arr, b, esz);
	
	memcpy(elem_tmp, pa, esz);
	memcpy(pa, pb, esz);
	memcpy(pb, elem_tmp, esz);
}

void arr_void_reverse(arr_void_info * inf, void * elem_tmp)
{
	size_t last = inf->end_non_incl-1;
	for (size_t i = inf->start; i < last; ++i, --last)
		arr_void_swap(inf, i, last, elem_tmp);
}

typedef struct if_not {
	arr_fpred pred;
	void * arg;
	void * found;
} if_not;
static bool apply_find_if_not(void * key, void * arg)
{
	if_not * inp = (if_not *)arg;
	if (!inp->pred(key, inp->arg))
	{
		inp->found = key;
		return false;
	}
	return true;
}
void * arr_void_partition(
	arr_void_info * inf,
	arr_fpred pred,
	void * arg,
	void * elem_tmp
)
{
	void * ret = NULL;
	if_not in = {pred, arg, NULL}; 
	
	arr_void_apply(inf, apply_find_if_not, &in);
	
	unsigned char * pfirst = (unsigned char *)in.found;
	if (pfirst)
	{
		void * arr = inf->data;
		size_t esz = inf->elem_size;
		size_t end = inf->end_non_incl;
		
		size_t first = arr_void_distance(inf, pfirst);
		size_t next = first + 1;
		unsigned char * pnext = pfirst + esz;
		
		while (next < end)
		{
			if (pred(pnext, arg))
			{
				arr_void_swap(inf, first, next, elem_tmp);
				++first;
			}
			++next;
			pnext += esz;
		}
		
		ret = offset(arr, first, esz);
	}
	
	return ret;
}

void arr_void_apply(arr_void_info * inf, arr_fapply fn, void * arg)
{
	decls();
	unsigned char * curr = offset(arr, start, esz);
	unsigned char * pend = offset(arr, end, esz);
	
	while (curr < pend && fn(curr, arg))
		curr += esz;
}
