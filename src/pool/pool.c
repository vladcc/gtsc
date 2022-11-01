#include "pool.h"
#include "misc/misc.h"

#include <string.h>
#include <stdlib.h>

static inline void zero_out(pool_void * pool)
{
	memset(pool, 0, sizeof(*pool));
}

#define next_sz (sizeof(void *))

static inline size_t align_to_ptr(size_t sz)
{
	return misc_align_to(sz, next_sz);
}

static pool_void * allocate_pool(pool_void * pool)
{	
	pool_void * ret = NULL;
	size_t elem_num = pool->elem_num;
	size_t real_elem_size = pool->elem_size;
	size_t real_pool_size = align_to_ptr(real_elem_size * elem_num) + next_sz;
	
	void * mem = malloc(real_pool_size);
	if (mem)
	{
		nslist_void * free_list = &pool->free_list;
		pool_elem_ctor ctor = pool->ctor;
		void * ctor_arg = pool->ctor_arg;
		
		unsigned char * first = (unsigned char *)mem;
		unsigned char * next = NULL;
		unsigned char * last = first + ((elem_num - 1) * real_elem_size);
		size_t next_offs = real_elem_size - next_sz;
		
		if (ctor)
		{
			while (first <= last)
			{
				ctor(first, ctor_arg);
				next = first + real_elem_size;
				*misc_offset_in(first, next_offs) = next;
				first = next;
			}
		}
		else
		{
			while (first <= last)
			{
				next = first + real_elem_size;
				*misc_offset_in(first, next_offs) = next;
				first = next;
			}
		}
		*misc_offset_in(last, next_offs) = NULL;
		nslist_void_set(free_list, mem);
			
		nslist_void_push(&pool->pool_list, mem);
		ret = pool;
	}
	
	return ret;
}

pool_void * pool_void_create(
	pool_void * pool,
	size_t elem_size,
	size_t elem_num,
	pool_elem_ctor ctor,
	void * ctor_arg,
	pool_elem_dtor dtor
)
{
	zero_out(pool);
	size_t real_elem_size = align_to_ptr(elem_size) + next_sz;
	size_t real_pool_size = align_to_ptr(real_elem_size * elem_num) + next_sz;
	
	nslist_void_init(&pool->free_list, real_elem_size - next_sz);
	pool->elem_size = real_elem_size;
	pool->elem_num = elem_num;
	nslist_void_init(&pool->pool_list, real_pool_size - next_sz);
	pool->ctor = ctor;
	pool->ctor_arg = ctor_arg;
	pool->dtor = dtor;
	
	return allocate_pool(pool);
}

void pool_void_destroy(pool_void * pool)
{
	size_t elem_num = pool->elem_num;
	size_t real_elem_size = pool->elem_size;

	nslist_void * pool_list = &pool->pool_list;
	void * mem = nslist_void_head(pool_list);
	void * next = NULL;
	
	while (mem)
	{
		pool_elem_dtor dtor = pool->dtor;

		if (dtor)
		{		
			unsigned char * first = (unsigned char *)mem;
			unsigned char * last = first + ((elem_num - 1) * real_elem_size);
			while (first <= last)
			{
				dtor(first);
				first += real_elem_size;
			}
		}

		next = nslist_void_next(pool_list, mem);
		free(mem);
		mem = next;
	}
	zero_out(pool);
}

void * pool_void_get(pool_void * pool)
{
	nslist_void * free_list = &pool->free_list;
	void * elem = nslist_void_pop(free_list);
	if (!elem)
	{
		if (allocate_pool(pool))
			elem = nslist_void_pop(free_list);
	}
	return elem;
}

void pool_void_put_back(pool_void * pool, void * elem)
{
	nslist_void_push(&pool->free_list, elem);
}
