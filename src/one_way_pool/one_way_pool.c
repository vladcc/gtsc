#include "one_way_pool.h"
#include "misc/misc.h"

#include <string.h>
#include <stdlib.h>

static inline void zero_out(owp_void * pool)
{
	memset(pool, 0, sizeof(*pool));
}

#define next_sz (sizeof(void *))

static inline size_t align_to_ptr(size_t sz)
{
	return misc_align_to(sz, next_sz);
}

static owp_void * allocate_pool(owp_void * pool)
{
	owp_void * ret = NULL;
	size_t elem_num = pool->elem_num;
	size_t elem_size = pool->elem_size;
	size_t real_pool_size = align_to_ptr(elem_size * elem_num) + next_sz;
	
	void * mem = malloc(real_pool_size);
	if (mem)
	{
		nslist_void_push(&pool->pool_list, mem);
		pool->pos = 0;
		ret = pool;
	}
	
	return ret;
}

owp_void * owp_void_create(owp_void * pool, size_t elem_size, size_t elem_num)
{
	zero_out(pool);
	pool->elem_size = elem_size;
	pool->elem_num = elem_num;

	size_t real_pool_size = align_to_ptr(elem_size * elem_num) + next_sz;
	nslist_void_init(&pool->pool_list, real_pool_size - next_sz);

	return allocate_pool(pool);
}

void owp_void_destroy(owp_void * pool)
{
	nslist_void * pool_list = &pool->pool_list;
	void * mem = nslist_void_head(pool_list);
	void * next = NULL;
	
	while (mem)
	{
		next = *nslist_void_pnext(pool_list, mem);
		free(mem);
		mem = next;
	}
	zero_out(pool);
}

void * owp_void_get(owp_void * pool)
{
	if (pool->elem_num <= pool->pos)
		allocate_pool(pool);

	void * elem = NULL;
	unsigned char * mem = (unsigned char *)nslist_void_head(&pool->pool_list);
	if (mem)
	{
		elem = (void *)(mem + pool->pos * pool->elem_size);
		++pool->pos;
	}
	return elem;
}
