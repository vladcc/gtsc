#include "grow_pool.h"
#include "grow_pool_priv.h"
#include "misc/misc.h"

#include <string.h>

#define size_t_sz() sizeof(size_t)

typedef unsigned char byte;

static inline void zero_out(grow_pool_void * pool)
{
	memset(pool, 0, sizeof(*pool));
}

static inline void set_next(void * elem, size_t elem_size, size_t next)
{
	*((size_t *)misc_offset_in(elem, elem_size - size_t_sz())) = next;
}

static inline size_t get_next(void * elem, size_t elem_size)
{
	return *((size_t *)misc_offset_in(elem, elem_size - size_t_sz()));
}

void gpool_set_next(grow_pool_void * pool, gpool_id id, gpool_id next)
{
	set_next(gpool_void_reach(pool, id), gpool_void_elem_size(pool), next.id);
}

gpool_id gpool_get_next(grow_pool_void * pool, gpool_id id)
{
	gpool_id ret = {
		get_next(gpool_void_reach(pool, id), gpool_void_elem_size(pool))
	};
	return ret;
}

static void init(grow_pool_void * pool, size_t start)
{	
	vect_void * vect = &pool->vect;
	size_t elem_size = vect_void_elem_size(vect);
	size_t cap = vect_void_cap(vect);
	byte * pelem = (byte *)vect_void_get(vect, start);
	
	gpool_elem_ctor ctor = (gpool_elem_ctor)pool->ctor;
	
	if (ctor)
	{
		void  * ctor_arg = pool->ctor_arg;
		for (size_t i = start; i < cap; ++i)
		{
			ctor(pelem, ctor_arg);
			set_next(pelem, elem_size, i+1);
			pelem += elem_size;
		}
	}
	else
	{
		for (size_t i = start; i < cap; ++i)
		{
			set_next(pelem, elem_size, i+1);
			pelem += elem_size;
		}
	}
	
	set_next(pelem-elem_size, elem_size, GPOOL_NO_NEXT);
}

grow_pool_void * gpool_void_create_sz(
	grow_pool_void * pool,
	size_t elem_size,
	size_t elem_num,
	gpool_elem_ctor ctor,
	void * ctor_arg,
	gpool_elem_dtor dtor
)
{
	zero_out(pool);
	
	vect_void * vect = &pool->vect;
	size_t real_elem_size = misc_align_to(elem_size, size_t_sz()) + size_t_sz();
	
	if (vect_void_create_cap(vect, real_elem_size, elem_num))
	{
		pool->ctor = (fvoid)ctor;
		pool->ctor_arg = ctor_arg;
		pool->dtor = (fvoid)dtor;
		init(pool, 0);
		return pool;
	}
	
	return NULL;
}

void gpool_void_destroy(grow_pool_void * pool)
{
	vect_void * vect = &pool->vect;
	gpool_elem_dtor dtor = (gpool_elem_dtor)pool->dtor;
	
	if (dtor)
	{
		
		size_t cap = vect_void_cap(vect);
		size_t elem_size = vect_void_elem_size(vect);
		byte * pelem = (byte *)vect_void_data(vect);
		byte * end = pelem + (cap * elem_size);
	
		while (pelem < end)
		{
			dtor(pelem);
			pelem += elem_size;
		}
	}
	
	vect_void_destroy(vect);
	zero_out(pool);
}

gpool_id gpool_void_get(grow_pool_void * pool)
{
	vect_void * vect = &pool->vect;
	size_t elem_size = vect_void_elem_size(vect);
	
	size_t head = vect_void_len(vect);
	if (GPOOL_NO_NEXT == head)
	{
		size_t cap = vect_void_cap(vect);
		vect_void_resize(vect, cap*2);
		init(pool, cap);
		head = cap;
	}
	
	vect_void_set_len(vect, get_next(vect_void_get(vect, head), elem_size));
	
	gpool_id ret = {head};
	return ret;
}

void gpool_void_put_back(grow_pool_void * pool, gpool_id id)
{
	vect_void * vect = &pool->vect;
	size_t elem_size = vect_void_elem_size(vect);
	
	size_t n = id.id;
	size_t head = vect_void_len(vect);
	set_next(vect_void_get(vect, n), elem_size, head);
	vect_void_set_len(vect, n);
}
