#include "arr_algo/arr_algo.h"
#include "vect/vect.h"
#include <stdlib.h>

static inline arr_void_info vect_void_range(
	const vect_void * v,
	size_t start,
	size_t end_non_incl
)
{
	arr_void_info vinf;
	arr_void_info_set(&vinf, v->data, v->elem_size, start, end_non_incl);
	return vinf;
}

static inline void * grow(vect_void * v)
{
	return vect_void_resize(v, v->cap*2);
}

static inline void zero_out(vect_void * v)
{
	memset(v, 0, sizeof(*v));
}

vect_void * vect_void_resize(vect_void * v, size_t cap)
{
    if (cap > 0)
    {
        void * result = realloc(v->data, cap*v->elem_size);
        if (result)
        {
            v->data = result;
            v->cap = cap;
            if (v->len > cap)
                v->len = cap;
            
            return v;
        }
    }

    return NULL;
}

vect_void * vect_void_create_cap(
	vect_void * v,
	size_t elem_size,
	size_t cap
)
{
	v->data = NULL;
	v->len = 0;
	v->cap = cap;
	v->elem_size = elem_size;
	
    if (vect_void_resize(v, cap))
		return v;

    zero_out(v);
    return NULL;
}

void vect_void_destroy(vect_void * v)
{
	free(v->data);
	zero_out(v);
}

vect_void * vect_void_append(vect_void * v, const void * src, size_t num)
{
	size_t new_len = v->len + num;
	size_t cap = v->cap;
	
	if (cap < new_len)
	{
		do {
			cap *= 2;
		} while (cap < new_len);
		
		if (!vect_void_resize(v, cap))
			return NULL;
	}
	
	arr_void_info inf;
	arr_void_info_set(&inf, (void *)src, v->elem_size, 0, num);
	arr_void_copy(&inf, vect_void_get(v, v->len));
	v->len = new_len;
	return v;
}

void * vect_void_push(vect_void * v, const void * elem)
{
	if (v->cap < v->len+1)
	{
		if (!grow(v))
			return NULL;
	}	
	
	return vect_void_write(v, v->len++, elem);
}

void vect_void_range_erase(vect_void * v, size_t start, size_t end_non_incl)
{
	arr_void_info inf = vect_void_range(v, end_non_incl, v->len);
	arr_void_move(&inf, vect_void_get(v, start));
	v->len -= end_non_incl - start;
}

void vect_void_range_erase_if(
	vect_void * v,
	size_t start,
	size_t end_non_incl,
	arr_fpred pred,
	void * arg
)
{
	size_t esz = v->elem_size;
	unsigned char * pstart = (unsigned char *)v->data + start * esz;
	unsigned char * pend = (unsigned char *)v->data + end_non_incl * esz;
	
	while (pstart < pend)
	{
		if (pred(pstart, arg))
		{
			vect_void_erase(v, start);
			pend -= esz;
		}
		else
		{
			++start;
			pstart += esz;
		}
	}
}

// algo
void vect_void_range_set_to(
	vect_void * v,
	size_t start,
	size_t end_non_incl,
	const void * key
)
{
	arr_void_info inf = vect_void_range(v, start, end_non_incl);
	arr_void_set_to(&inf, key);
}

vect_void vect_void_range_copy(
	const vect_void * src,
	size_t start,
	size_t end_non_incl,
	bool * out_ok
)
{
	*out_ok = false;
	vect_void cpy;
	size_t cpy_len = end_non_incl - start;
	if (vect_void_create_cap(&cpy, src->elem_size, cpy_len))
	{
		arr_void_info inf = vect_void_range(src, start, end_non_incl);
		arr_void_copy(&inf, cpy.data);
		cpy.len = cpy_len;
		*out_ok = true;
	}
	return cpy;
}

bool vect_void_range_is_sorted(
	const vect_void * v,
	size_t start,
	size_t end_non_incl,
	arr_fcomp cmp
)
{
	arr_void_info inf = vect_void_range(v, start, end_non_incl);
	return arr_void_is_sorted(&inf, cmp);
}

void vect_void_range_sort(
	vect_void * v,
	size_t start,
	size_t end_non_incl,
	arr_fcomp cmp
)
{
	arr_void_info inf = vect_void_range(v, start, end_non_incl);
	arr_void_sort(&inf, cmp);
}

void * vect_void_range_find(                                                   
    const vect_void * v,                                                       
    size_t start,                                                              
    size_t end_non_incl,                                                       
    const void * key,                                                          
    arr_fcomp cmp                                                                  
)                                                                              
{                                                                              
    arr_void_info inf = vect_void_range(v, start, end_non_incl);                    
    return arr_void_find(&inf, key, cmp);                                           
}                                                                              

const void * vect_void_range_bsearch(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
)
{
    arr_void_info inf = vect_void_range(v, start, end_non_incl);
    return arr_void_bsearch(&inf, key, cmp);
}

const void * vect_void_range_sfirst_eq(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
)
{
    arr_void_info inf = vect_void_range(v, start, end_non_incl);
    return arr_void_sfirst_eq(&inf, key, cmp);
}

const void * vect_void_range_sfirst_geq(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
)
{
    arr_void_info inf = vect_void_range(v, start, end_non_incl);
    return arr_void_sfirst_geq(&inf, key, cmp);
}

const void * vect_void_range_sfirst_gt(
    const vect_void * v,
    size_t start,
    size_t end_non_incl,
    const void * key,
    arr_fcomp cmp
)
{
    arr_void_info inf = vect_void_range(v, start, end_non_incl);
    return arr_void_sfirst_gt(&inf, key, cmp);
}

void vect_void_swap(vect_void * v, size_t a, size_t b, void * elem_tmp)
{
	arr_void_info inf = vect_void_range(v, 0, 0);
	arr_void_swap(&inf, a, b, elem_tmp);
}

void vect_void_range_reverse(
	vect_void * v,
	size_t start,
	size_t end_non_incl,
	void * elem_tmp
)
{
	arr_void_info inf = vect_void_range(v, start, end_non_incl);
	arr_void_reverse(&inf, elem_tmp);
}

void vect_void_range_apply(
	vect_void * v,
	size_t start,
	size_t end_non_incl,
	arr_fapply fn,
	void * arg
)
{
	arr_void_info inf = vect_void_range(v, start, end_non_incl);
	arr_void_apply(&inf, fn, arg);
}

void * vect_void_range_partition(
	vect_void * v,
	size_t start,
	size_t end_non_incl,
	arr_fpred pred,
	void * arg,
	void * elem_tmp
)
{
	arr_void_info inf = vect_void_range(v, start, end_non_incl);
	return arr_void_partition(&inf, pred, arg, elem_tmp);
}

bool vect_void_is_eq(const vect_void * a, const vect_void * b, arr_fcomp cmp)
{
	arr_void_info infa = vect_void_range(a, 0, a->len);
	arr_void_info infb = vect_void_range(b, 0, b->len);
	return arr_void_is_eq(&infa, &infb, cmp);
}

void * vect_void_insert(vect_void * v, size_t index, const void * elem)
{
	if (v->cap < v->len+1)
	{
		if (!grow(v))
			return NULL;
	}

	arr_void_info inf = vect_void_range(v, index, v->len);
	arr_void_move(&inf, vect_void_get(v, index+1));
	
	++v->len;
	return vect_void_write(v, index, elem);
}

vect_void * vect_void_insert_n(
	vect_void * v,
	size_t index,
	const void * src,
	size_t num
)
{
	size_t new_len = v->len + num;
	size_t cap = v->cap;
	
	if (cap < new_len)
	{
		do {
			cap *= 2;
		} while (cap < new_len);
		
		if (!vect_void_resize(v, cap))
			return NULL;
	}

	arr_void_info inf = vect_void_range(v, index, v->len);
	arr_void_move(&inf, vect_void_get(v, index+num));
	
	arr_void_info_set(&inf, (void *)src, v->elem_size, 0, num);
	arr_void_copy(&inf, vect_void_get(v, index));
	
	v->len = new_len;
	return v;
}
