#include "ch_set_priv.h"
#include "misc/misc.h"

#include <stdlib.h>

#define next_sz (sizeof(void *))

static inline size_t align_to_ptr(size_t sz)
{
	return misc_align_to(sz, next_sz);
}

static chs_priv_pool * allocate_pool(chs_priv_pool * pool)
{	
	chs_priv_pool * ret = NULL;
	size_t elem_num = pool->elem_num;
	size_t real_elem_size = pool->elem_size;
	size_t real_pool_size = align_to_ptr(real_elem_size * elem_num) + next_sz;
	
	void * mem = malloc(real_pool_size);
	if (mem)
	{
		nslist_void * free_list = &pool->free_list;
		
		unsigned char * first = (unsigned char *)mem;
		unsigned char * next = NULL;
		unsigned char * last = first + ((elem_num - 1) * real_elem_size);
		size_t next_offs = real_elem_size - next_sz;
		
		while (first <= last)
		{
			next = first + real_elem_size;
			*misc_offset_in(first, next_offs) = next;
			first = next;
		}
			
		*misc_offset_in(last, next_offs) = NULL;
		nslist_void_set(free_list, mem);
			
		nslist_void_push(&pool->pool_list, mem);
		ret = pool;
	}
	
	return ret;
}

chs_priv_pool * chs_priv_pool_create(
	chs_priv_pool * pool,
	size_t elem_size,
	size_t elem_num
)
{
	size_t real_elem_size = align_to_ptr(elem_size) + next_sz;
	size_t real_pool_size = align_to_ptr(real_elem_size * elem_num) + next_sz;
	
	nslist_void_init(&pool->free_list, real_elem_size - next_sz);
	pool->elem_size = real_elem_size;
	pool->elem_num = elem_num;
	nslist_void_init(&pool->pool_list, real_pool_size - next_sz);
	
	return allocate_pool(pool);
}

void chs_priv_pool_destroy(chs_priv_pool * pool)
{
	nslist_void * pool_list = &pool->pool_list;
	void * mem = nslist_void_head(pool_list);
	void * next = NULL;
	
	while (mem)
	{
		next = nslist_void_next(pool_list, mem);
		free(mem);
		mem = next;
	}
}

void * chs_priv_pool_get(chs_priv_pool * pool)
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

void chs_priv_pool_put_back(chs_priv_pool * pool, void * elem)
{
	nslist_void_push(&pool->free_list, elem);
}

void * chs_priv_get_elem_addr(chs_priv_pool * pool, size_t elem_num)
{
	size_t count = 0;
	size_t pl_elem_count = pool->elem_num;
	size_t pl_elem_size = pool->elem_size;
	nslist_void * pool_list = &pool->pool_list;
	
	void * pl = nslist_void_head(pool_list);
	while (pl)
	{
		for (size_t i = 0; i < pl_elem_count; ++i)
		{
			if (count == elem_num)
				return (unsigned char *)pl + (i * pl_elem_size);
			++count;
		}
		pl = nslist_void_next(pool_list, pl);
	}
	
	return NULL;
}
