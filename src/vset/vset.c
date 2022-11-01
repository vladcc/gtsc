#include "vset.h"

vset_void * vset_void_create_cap(
	vset_void * set,
	size_t elem_size,
	arr_fcomp compar,
	size_t cap
)
{
	if (vect_void_create_cap(&set->vect, elem_size, cap))
	{
		set->compar = compar;
		return set;
	}
	return NULL;
} 

void vset_void_destroy(vset_void * set)
{
	vect_void_destroy(&set->vect);
	memset(set, 0, sizeof(*set));
}

vset_void vset_void_copy(const vset_void * src, bool * is_ok)
{
	vset_void vs;
	vs.compar = src->compar;
	vs.vect = vect_void_copy(&src->vect, is_ok);
	return vs;
}

void vset_void_insert(vset_void * set, const void * elem)
{
	vect_void * v = &set->vect;
	const void * place = vect_void_sfirst_geq(v, elem, set->compar);
	
	if (place)
	{
		if (set->compar(place, elem) != 0)
			vect_void_insert(v, vect_void_distance(v, place), elem);
	}
	else
		vect_void_push(v, elem);
}

vset_void * vset_void_insert_arr(
	vset_void * set,
	const void * arr,
	size_t len
)
{
	size_t elem_size = vect_void_elem_size(&set->vect);
	const unsigned char * first = (const unsigned char *)arr;
	const unsigned char * end = (const unsigned char *)arr + len * elem_size;
	
	while (first < end)
	{
		vset_void_insert(set, first);
		first += elem_size;
	}
	
	return set;
}

void vset_void_erase(vset_void * set, const void * val)
{
	const void * find = vset_void_has(set, val);
	if (find)
	{
		vect_void * v = &set->vect;
		vect_void_erase(v, vect_void_distance(v, find));
	}
}

vset_void * vset_void_diff(
	vset_void * diff,
	const vset_void * set_a,
	const vset_void * set_b
)
{
	vset_void_reset(diff);
	
	const void * elem = NULL;
	for (size_t i = 0, end = vset_void_size(set_a); i < end; ++i)
	{
		elem = vset_void_get(set_a, i);
		if (!vset_void_has(set_b, elem))
			vset_void_insert(diff, elem);
	}
	
	return diff;
}

vset_void * vset_void_union(
	vset_void * un,
	const vset_void * set_a,
	const vset_void * set_b
)
{
	vset_void_reset(un);
	
	for (size_t i = 0, end = vset_void_size(set_a); i < end; ++i)
		vset_void_insert(un, vset_void_get(set_a, i));
		
	for (size_t i = 0, end = vset_void_size(set_b); i < end; ++i)
		vset_void_insert(un, vset_void_get(set_b, i));
	
	return un;
}

vset_void * vset_void_intersect(
	vset_void * isc,
	const vset_void * set_a,
	const vset_void * set_b
)
{
	vset_void_reset(isc);
	
	const void * elem = NULL;
	for (size_t i = 0, end = vset_void_size(set_a); i < end; ++i)
	{
		elem = vset_void_get(set_a, i);
		if (vset_void_has(set_b, elem))
			vset_void_insert(isc, elem);
	}
	
	return isc;
}
