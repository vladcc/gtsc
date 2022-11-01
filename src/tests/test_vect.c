#include <stdio.h>
#include "test.h"
#include "vect/vect.h"

#define ASIZE 5

static bool test_vect_void_create(void)
{
	vect_void zero_, * zero = &zero_;
	memset(zero, 0, sizeof(*zero));
	
	vect_void vv_, * vv = &vv_;
	memset(vv, 0, sizeof(*vv));
	check(memcmp(vv, zero, sizeof(*vv)) == 0);
	
	check(vect_void_create(vv, sizeof(int)) == vv);
	check(memcmp(vv, zero, sizeof(*vv)) != 0);
	check(vv->data);
	check(0 == vv->len);
	check(32 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	vect_void_destroy(vv);
	check(memcmp(vv, zero, sizeof(*vv)) == 0);
	return true;
}

static bool test_vect_void_create_arr(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_void vv_, * vv = &vv_;
	check(vect_void_create_arr(vv, sizeof(int), arr, ASIZE) == vv);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	vect_void_destroy(vv);
	return true;
}

static bool test_vect_void_resize(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_void vv_, * vv = &vv_;
	vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_void_resize(vv, ASIZE*2) == vv);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE*2 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_void_resize(vv, 3) == vv);
	check(vv->data);
	check(3 == vv->len);
	check(3 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, 3*sizeof(int)) == 0);
	
	vect_void_destroy(vv);
	return true;
}

static bool test_vect_void_shrink_to_fit(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_void vv_, * vv = &vv_;
	vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_void_resize(vv, ASIZE*2) == vv);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE*2 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_void_shrink_to_fit(vv) == vv);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);

	vect_void_reset(vv);
	check(0 == vv->len);
	check(!vect_void_shrink_to_fit(vv));
	check(vv->data);
	check(0 == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	vect_void_destroy(vv);
	return true;
}

static bool test_vect_void_getters(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_void vv_, * vv = &vv_;
	vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_void_data(vv) == vv->data);
	check(vect_void_len(vv) == vv->len);
	check(vect_void_cap(vv) == vv->cap);
	check(vect_void_elem_size(vv) == vv->elem_size);
	
	vect_void_destroy(vv);
	return true;
}

static bool test_vect_void_is_empty(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_void vv_, * vv = &vv_;
	check(vect_void_create(vv, sizeof(int)) == vv);
	check(vv->data);
	check(0 == vv->len);
	check(32 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	check(vect_void_is_empty(vv));
	
	vect_void_destroy(vv);
	
	vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	check(!vect_void_is_empty(vv));
	
	vect_void_destroy(vv);
	return true;
}

static bool test_vect_void_last_get_set_len_zero_distance(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_void vv_, * vv = &vv_;
	vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_void_last(vv) == 4);
	check(1 == *((int *)vect_void_get(vv, 0)));
	check(3 == *((int *)vect_void_get(vv, 2)));
	check(5 == *((int *)vect_void_get(vv, vect_void_last(vv))));
	
	check(0 == vect_void_distance(vv, vect_void_get(vv, 0)));
	check(1 == vect_void_distance(vv, vect_void_get(vv, 1)));
	check(2 == vect_void_distance(vv, vect_void_get(vv, 2)));
	check(3 == vect_void_distance(vv, vect_void_get(vv, 3)));
	check(4 == vect_void_distance(vv, vect_void_get(vv, 4)));

	vect_void_set_len(vv, 3);
	check(vv->data);
	check(3 == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, 3*sizeof(int)) == 0);
	
	check(vect_void_last(vv) == 2);
	check(1 == *((int *)vect_void_get(vv, 0)));
	check(3 == *((int *)vect_void_get(vv, 2)));
	check(3 == *((int *)vect_void_get(vv, vect_void_last(vv))));
	
	int arr_zero_len[3] = {0, 0, 0};
	int arr_zero_cap[ASIZE] = {0, 0, 0, 0, 0};
	
	check(!memcmp(vv->data, arr_zero_len, 3*sizeof(int)) == 0);
	check(!memcmp(vv->data, arr_zero_cap, ASIZE*sizeof(int)) == 0);
	
	vect_void_len_zero(vv);
	check(memcmp(vv->data, arr_zero_len, 3*sizeof(int)) == 0);
	check(!memcmp(vv->data, arr_zero_cap, ASIZE*sizeof(int)) == 0);
	
	vect_void_cap_zero(vv);
	check(memcmp(vv->data, arr_zero_len, 3*sizeof(int)) == 0);
	check(memcmp(vv->data, arr_zero_cap, ASIZE*sizeof(int)) == 0);
	
	vect_void_destroy(vv);
	return true;
}

static bool test_vect_void_push_pop_peek_write_pop_n(void)
{
	int elem = 5;
	
	vect_void vv_, * vv = &vv_;
	check(vect_void_create_cap(vv, sizeof(int), 1) == vv);
	
	check(vv->data);
	check(0 == vv->len);
	check(1 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	check(5 == *((int *)vect_void_push(vv, &elem)));
	check(5 == *((int *)vect_void_get(vv, 0)));
	check(vv->data);
	check(1 == vv->len);
	check(1 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	elem = 6;
	check(6 == *((int *)vect_void_write(vv, 0, &elem)));
	
	elem = 7;
	check(7 == *((int *)vect_void_push(vv, &elem)));
	check(7 == *((int *)vect_void_peek(vv)));
	check(vv->data);
	check(2 == vv->len);
	check(2 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	elem = 8;
	check(8 == *((int *)vect_void_write(vv, 1, &elem)));
	check(8 == *((int *)vect_void_peek(vv)));
	
	elem = 9;
	check(9 == *((int *)vect_void_push(vv, &elem)));
	check(9 == *((int *)vect_void_peek(vv)));
	check(vv->data);
	check(3 == vv->len);
	check(4 == vv->cap);
	check(sizeof(int) == vv->elem_size);

	vect_void_pop(vv);
	check(8 == *((int *)vect_void_peek(vv)));
	check(vv->data);
	check(2 == vv->len);
	check(4 == vv->cap);
	check(sizeof(int) == vv->elem_size);

	check(9 == *((int *)vect_void_push(vv, &elem)));
	check(vv->data);
	check(3 == vv->len);
	check(4 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	check(9 == *((int *)vect_void_push(vv, &elem)));
	check(vv->data);
	check(4 == vv->len);
	check(4 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	check(9 == *((int *)vect_void_push(vv, &elem)));
	check(vv->data);
	check(5 == vv->len);
	check(8 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	vect_void_pop_n(vv, 3);
	check(8 == *((int *)vect_void_peek(vv)));
	check(vv->data);
	check(2 == vv->len);
	check(8 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	vect_void_destroy(vv);
	return true;
}

static bool test_vect_void_swap_pop(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_void vv_, * vv = &vv_;
	vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
	check(vv->data);
	check(ASIZE == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
	
	int buff;
	vect_void_swap_pop(vv, 2, &buff);
	
	int arr_1[ASIZE-1] = {1, 2, 5, 4};
	check(ASIZE-1 == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr_1, (ASIZE-1)*sizeof(int)) == 0);
	
	int arr_2[ASIZE-2] = {4, 2, 5};
	vect_void_swap_pop(vv, 0, &buff);
	check(ASIZE-2 == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr_2, (ASIZE-2)*sizeof(int)) == 0);
	
	int arr_3[ASIZE-3] = {4, 2};
	vect_void_swap_pop(vv, vect_void_last(vv), &buff);
	check(ASIZE-3 == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr_3, (ASIZE-3)*sizeof(int)) == 0);
	
	int arr_4[ASIZE-4] = {2};
	vect_void_swap_pop(vv, 0, &buff);
	check(ASIZE-4 == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(memcmp(vv->data, arr_4, (ASIZE-4)*sizeof(int)) == 0);
	check(!vect_void_is_empty(vv));
	
	vect_void_swap_pop(vv, 0, &buff);
	check(0 == vv->len);
	check(ASIZE == vv->cap);
	check(sizeof(int) == vv->elem_size);
	check(vect_void_is_empty(vv));
	
	vect_void_destroy(vv);
	return true;
}

static bool erase_pred(const void * key, void * arg)
{
	return *((int *)key) == *((int *)arg);
}
static bool test_vect_void_erase(void)
{	
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	vect_void vv_, * vv = &vv_;
	
	{
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int arr_2[ASIZE-1] = {2, 3, 4, 5};
		vect_void_erase(vv, 0);
		check(ASIZE-1 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_2, (ASIZE-1)*sizeof(int)) == 0);
		
		int arr_3[ASIZE-2] = {2, 3, 4};
		vect_void_erase(vv, vect_void_last(vv));
		check(ASIZE-2 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_3, (ASIZE-2)*sizeof(int)) == 0);
		
		int arr_4[ASIZE-3] = {2, 4};
		vect_void_erase(vv, 1);
		check(ASIZE-3 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_4, (ASIZE-3)*sizeof(int)) == 0);
		vect_void_destroy(vv);
	}
	
	{
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase(vv, 0, 3);
		int arr_2[ASIZE-2] = {4, 5};
		check(ASIZE-3 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_2, (ASIZE-3)*sizeof(int)) == 0);
		vect_void_destroy(vv);
		
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase(vv, 3, vect_void_len(vv));
		int arr_3[ASIZE-2] = {1, 2, 3};
		check(ASIZE-2 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_3, (ASIZE-2)*sizeof(int)) == 0);
		vect_void_destroy(vv);
		
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase(vv, 1, vect_void_last(vv));
		int arr_4[ASIZE-2] = {1, 5};
		check(ASIZE-3 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_4, (ASIZE-3)*sizeof(int)) == 0);
		vect_void_destroy(vv);
	}
	
	{
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase(vv, 0, 0);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase(vv, 0, 1);
		int arr_2[ASIZE-1] = {2, 3, 4, 5};
		check(ASIZE-1 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_2, (ASIZE-1)*sizeof(int)) == 0);
		vect_void_destroy(vv);
		
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase(vv, vect_void_last(vv), vect_void_len(vv));
		int arr_3[ASIZE-1] = {1, 2, 3, 4};
		check(ASIZE-1 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_3, (ASIZE-1)*sizeof(int)) == 0);
		vect_void_destroy(vv);
		
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase(vv, 1, 2);
		int arr_4[ASIZE-1] = {1, 3, 4, 5};
		check(ASIZE-1 == vv->len);
		check(ASIZE == vv->cap);
		check(memcmp(vv->data, arr_4, (ASIZE-3)*sizeof(int)) == 0);
		vect_void_destroy(vv);
	}
	
	{
		#undef ASIZE
		#define ASIZE 11
		
		int arr_1[ASIZE] = {0, 0, 1, 2, 0, 3, 0, 4, 5, 0, 0};
		
		vect_void_create_arr(vv, sizeof(int), arr_1, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr_1, ASIZE*sizeof(int)) == 0);
		
		size_t new_size = 5;
		int what = 0;
		vect_void_erase_if(vv, erase_pred, &what);
		check(vv->len == new_size);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr, new_size*sizeof(int)) == 0);
		vect_void_destroy(vv);
		
		vect_void_create_arr(vv, sizeof(int), arr_1, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr_1, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase_if(vv, 1, 1, erase_pred, &what);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr_1, ASIZE*sizeof(int)) == 0);
		
		int arr_2[ASIZE-1] = {0, 1, 2, 0, 3, 0, 4, 5, 0, 0}; 
		vect_void_range_erase_if(vv, 1, 2, erase_pred, &what);
		check(ASIZE-1 == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr_2, (ASIZE-1)*sizeof(int)) == 0);
		vect_void_destroy(vv);
		
		vect_void_create_arr(vv, sizeof(int), arr_1, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr_1, ASIZE*sizeof(int)) == 0);
		
		vect_void_range_erase_if(vv, 1, 4, erase_pred, &what);
		check(ASIZE-1 == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr_2, (ASIZE-1)*sizeof(int)) == 0);
		vect_void_destroy(vv);
		
		vect_void_create_arr(vv, sizeof(int), arr_1, ASIZE);
		check(vv->data);
		check(ASIZE == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr_1, ASIZE*sizeof(int)) == 0);
		
		int arr_3[ASIZE-2] = {0, 1, 2, 3, 0, 4, 5, 0, 0};
		vect_void_range_erase_if(vv, 1, 5, erase_pred, &what);
		check(ASIZE-2 == vv->len);
		check(ASIZE == vv->cap);
		check(sizeof(int) == vv->elem_size);
		check(memcmp(vv->data, arr_3, (ASIZE-2)*sizeof(int)) == 0);
		vect_void_destroy(vv);
		
		#undef ASIZE
		#define ASIZE 5
	}
	
	return true;
}

static bool test_vect_void_insert_append_reset(void)
{
	
	vect_void vv_, * vv = &vv_;
	check(vect_void_create_cap(vv, sizeof(int), 1) == vv);
	
	check(vv->data);
	check(0 == vv->len);
	check(1 == vv->cap);
	check(sizeof(int) == vv->elem_size);
	
	int elem = 5;
	check(5 == *((int *)vect_void_push(vv, &elem)));
	check(1 == vv->len);
	check(1 == vv->cap);
	
	elem = 3;
	check(3 == *((int *)vect_void_insert(vv, 0, &elem)));
	check(3 == *((int *)vect_void_get(vv, 0)));
	check(5 == *((int *)vect_void_get(vv, 1)));
	check(2 == vv->len);
	check(2 == vv->cap);
	
	elem = 4;
	check(4 == *((int *)vect_void_insert(vv, vect_void_last(vv), &elem)));
	check(3 == *((int *)vect_void_get(vv, 0)));
	check(4 == *((int *)vect_void_get(vv, 1)));
	check(5 == *((int *)vect_void_get(vv, 2)));
	check(3 == vv->len);
	check(4 == vv->cap);	
	
	int ins[3] = {7, 8, 9};
	int arr_1[6] = {7, 8, 9, 3, 4, 5};
	
	check(vv == vect_void_insert_n(vv, 0, ins, 3));
	check(6 == vv->len);
	check(8 == vv->cap);
	check(memcmp(vv->data, arr_1, 6*sizeof(int)) == 0);
	
	int arr_2[8] = {7, 8, 9, 3, 4, 7, 8, 5};
	check(vv == vect_void_insert_n(vv, vect_void_last(vv), ins, 2));
	check(8 == vv->len);
	check(8 == vv->cap);
	check(memcmp(vv->data, arr_2, 8*sizeof(int)) == 0);

	vect_void_reset(vv);
	check(0 == vv->len);
	check(8 == vv->cap);
	
	check(vv == vect_void_append(vv, ins, 2));
	check(2 == vv->len);
	check(8 == vv->cap);
	check(memcmp(vv->data, ins, 2*sizeof(int)) == 0);
	
	int arr_3[5] = {7, 8, 7, 8, 9};
	check(vv == vect_void_append(vv, ins, 3));
	check(5 == vv->len);
	check(8 == vv->cap);
	check(memcmp(vv->data, arr_3, 5*sizeof(int)) == 0);

	int ins_2[3] = {1, 2, 3};
	int arr_4[7] = {7, 8, 1, 2, 7, 8, 9};
	check(vv == vect_void_insert_n(vv, 2, ins_2, 2));
	check(7 == vv->len);
	check(8 == vv->cap);
	check(memcmp(vv->data, arr_4, 7*sizeof(int)) == 0);
	
	int arr_5[10] = {7, 8, 1, 2, 7, 8, 9, 1, 2, 3};
	check(vv == vect_void_append(vv, ins_2, 3));
	check(10 == vv->len);
	check(16 == vv->cap);
	check(memcmp(vv->data, arr_5, 10*sizeof(int)) == 0);
	
	vect_void_destroy(vv);
	return true;
}

static int compar(const void * k1, const void * k2)
{
	int a = *((const int *)k1);
	int b = *((const int *)k2);
	
	if (a < b)
		return -1;
	if (a > b)
		return 1;
	return 0;
}
static bool bump_up(void * arr_key, void * arg)
{
	int * pi = (int *)arr_key;
	*pi += *(int *)arg;
	return true;
}
static bool bump_first(void * arr_key, void * arg)
{
	int * pi = (int *)arr_key;
	*pi += *(int *)arg;
	return false;
}
static bool pred_part(const void * arr_key, void * arg)
{
	return (*((int *)arr_key) > *((int *)arg));
}
static bool test_vect_void_algo_wrappers(void)
{
	vect_void vv_, * vv = &vv_;
	
	{
		// vect_void_range_set_to, vect_void_set_to
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int arr_2[ASIZE] = {1, 7, 7, 7, 5};
		int key = 7;
		vect_void_range_set_to(vv, 1, vect_void_last(vv), &key);
		check(memcmp(vv->data, arr_2, ASIZE*sizeof(int)) == 0);
		
		int arr_3[ASIZE] = {8, 8, 8, 8, 8};
		key = 8;
		vect_void_set_to(vv, &key);
		check(memcmp(vv->data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_copy, vect_void_copy
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		bool res = false;
		vect_void vcp = vect_void_range_copy(vv, 1, 4, &res);
		check(res);
		check(vcp.data);
		check(vcp.len == 3);
		check(vcp.cap == 3);
		check(vcp.elem_size == sizeof(int));
		check(memcmp(vcp.data, arr+1, 3*sizeof(int)) == 0);
		
		vect_void_destroy(&vcp);
		check(vcp.data == NULL);
		check(vcp.len == 0);
		check(vcp.cap == 0);
		check(vcp.elem_size == 0);
		
		res = false;
		vcp = vect_void_copy(vv, &res);
		check(res);
		check(vcp.data);
		check(vcp.len == vv->len);
		check(vcp.cap == vv->cap);
		check(vcp.elem_size == vv->elem_size);
		check(memcmp(vcp.data, vv->data, vv->len*sizeof(int)) == 0);
		
		vect_void_destroy(&vcp);
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_is_sorted vect_void_is_sorted
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		check(vect_void_range_is_sorted(vv, 1, 1, compar));
		check(vect_void_range_is_sorted(vv, 1, 4, compar));
		check(vect_void_is_sorted(vv, compar));
		vect_void_destroy(vv);
		
		int arr_2[ASIZE] = {1, 2, 3, 5, 4};
		vect_void_create_arr(vv, sizeof(int), arr_2, ASIZE);
		check(memcmp(vv->data, arr_2, ASIZE*sizeof(int)) == 0);
		
		check(vect_void_range_is_sorted(vv, 1, 4, compar));
		check(!vect_void_range_is_sorted(vv, 1, 5, compar));
		check(!vect_void_is_sorted(vv, compar));
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_sort, vect_void_sort
		
		int arr[ASIZE] = {5, 4, 3, 2, 1};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		check(!vect_void_range_is_sorted(vv, 1, 4, compar));
		vect_void_range_sort(vv, 1, 4, compar);
		check(vect_void_range_is_sorted(vv, 1, 4, compar));
		
		int arr_2[ASIZE] = {5, 2, 3, 4, 1};
		check(memcmp(vv->data, arr_2, ASIZE*sizeof(int)) == 0);
		
		check(!vect_void_is_sorted(vv, compar));
		vect_void_sort(vv, compar);
		check(vect_void_is_sorted(vv, compar));
		
		int arr_3[ASIZE] = {1, 2, 3, 4, 5};
		check(memcmp(vv->data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_find, vect_void_find
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 1;
		check(!vect_void_range_find(vv, 1, 4, &key, compar));
		
		key = 3;
		void * res = vect_void_range_find(vv, 1, 4, &key, compar);
		check(res);
		check(vect_void_distance(vv, res) == 2);
		
		key = -1000;
		check(!vect_void_find(vv, &key, compar));
		
		key = 1;
		res = vect_void_find(vv, &key, compar);
		check(res);
		check(vect_void_distance(vv, res) == 0);
		
		
		key = 5;
		res = vect_void_find(vv, &key, compar);
		check(res);
		check(vect_void_distance(vv, res) == 4);
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_bsearch, vect_void_bsearch
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 1;
		check(!vect_void_range_bsearch(vv, 1, 4, &key, compar));
		
		key = 3;
		const void * res = vect_void_range_bsearch(vv, 1, 4, &key, compar);
		check(res);
		check(vect_void_distance(vv, res) == 2);
		
		key = -1000;
		check(!vect_void_bsearch(vv, &key, compar));
		
		key = 1;
		res = vect_void_bsearch(vv, &key, compar);
		check(res);
		check(vect_void_distance(vv, res) == 0);
		
		
		key = 5;
		res = vect_void_bsearch(vv, &key, compar);
		check(res);
		check(vect_void_distance(vv, res) == 4);
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_sfirst_eq, vect_void_sfirst_eq
		
		int arr[ASIZE] = {1, 2, 2, 5, 6};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 5;
		check(!vect_void_range_sfirst_eq(vv, 0, 3, &key, compar));
		
		key = 2;
		const void * res = vect_void_range_sfirst_eq(vv, 0, 3, &key, compar);
		check(res);
		check(1 == vect_void_distance(vv, res));
		
		res = vect_void_sfirst_eq(vv, &key, compar);
		check(res);
		check(1 == vect_void_distance(vv, res));
		
		key = 1;
		res = vect_void_sfirst_eq(vv, &key, compar);
		check(res);
		check(0 == vect_void_distance(vv, res));
		
		key = 5;
		res = vect_void_sfirst_eq(vv, &key, compar);
		check(res);
		check(3 == vect_void_distance(vv, res));
		
		key = 6;
		res = vect_void_sfirst_eq(vv, &key, compar);
		check(res);
		check(4 == vect_void_distance(vv, res));
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_sfirst_geq, vect_void_range_geq
		
		int arr[ASIZE] = {1, 2, 2, 5, 1000};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 3;
		check(!vect_void_range_sfirst_geq(vv, 0, 3, &key, compar));
		
		key = 2;
		const void * res = vect_void_range_sfirst_geq(vv, 0, 3, &key, compar);
		check(res);
		check(1 == vect_void_distance(vv, res));
		
		key = 1;
		res = vect_void_range_sfirst_geq(vv, 0, 3, &key, compar);
		check(res);
		check(0 == vect_void_distance(vv, res));
		
		key = -1000;
		res = vect_void_range_sfirst_geq(vv, 0, 3, &key, compar);
		check(res);
		check(0 == vect_void_distance(vv, res));
		
		key = 1001;
		check(!vect_void_sfirst_geq(vv, &key, compar));
		
		key = 900;
		res = vect_void_sfirst_geq(vv, &key, compar);
		check(res);
		check(4 == vect_void_distance(vv, res));
		
		key = 0;
		res = vect_void_sfirst_geq(vv, &key, compar);
		check(res);
		check(0 == vect_void_distance(vv, res));
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_sfirst_gt, vect_void_sfirst_gt
		
		int arr[ASIZE] = {1, 2, 2, 5, 1000};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 3;
		check(!vect_void_range_sfirst_gt(vv, 0, 3, &key, compar));
		
		key = 1;
		const void * res = vect_void_range_sfirst_gt(vv, 0, 3, &key, compar);
		check(res);
		check(1 == vect_void_distance(vv, res));
		
		key = 1;
		res = vect_void_range_sfirst_gt(vv, 0, 3, &key, compar);
		check(res);
		check(1 == vect_void_distance(vv, res));
		
		key = -1000;
		res = vect_void_range_sfirst_gt(vv, 0, 3, &key, compar);
		check(res);
		check(0 == vect_void_distance(vv, res));
		
		key = 1001;
		check(!vect_void_sfirst_gt(vv, &key, compar));
		
		key = 900;
		res = vect_void_sfirst_gt(vv, &key, compar);
		check(res);
		check(4 == vect_void_distance(vv, res));
		
		key = 0;
		res = vect_void_sfirst_gt(vv, &key, compar);
		check(res);
		check(0 == vect_void_distance(vv, res));
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_is_eq
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		bool is_ok = false;
		vect_void cpy = vect_void_copy(vv, &is_ok);
		check(is_ok);
		check(vect_void_is_eq(vv, &cpy, compar));
		
		int key = 10;
		vect_void_write(vv, 0, &key);
		check(!vect_void_is_eq(vv, &cpy, compar));
		
		vect_void_destroy(&cpy);
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_swap
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int buff;
		vect_void_swap(vv, 0, 0, &buff);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_void_swap(vv, 2, 2, &buff);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		check(1 == *((int *)vect_void_get(vv, 0)));
		check(5 == *((int *)vect_void_get(vv, vect_void_last(vv))));
		
		vect_void_swap(vv, 0, vect_void_last(vv), &buff);
		
		check(5 == *((int *)vect_void_get(vv, 0)));
		check(1 == *((int *)vect_void_get(vv, vect_void_last(vv))));
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_reverse, vect_void_reverse
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int buff;
		vect_void_range_reverse(vv, 1, 4, &buff);
		
		int arr_2[ASIZE] = {1, 4, 3 ,2 ,5};
		check(memcmp(vv->data, arr_2, ASIZE*sizeof(int)) == 0);
		
		int arr_3[ASIZE] = {5, 2, 3, 4, 1};
		vect_void_reverse(vv, &buff);
		check(memcmp(vv->data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_void_pop(vv);
		vect_void_reverse(vv, &buff);
		
		int arr_4[ASIZE-1] = {4, 3, 2, 5};
		check(memcmp(vv->data, arr_4, (ASIZE-1)*sizeof(int)) == 0);
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_apply, vect_void_apply
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int bump = 10;
		vect_void_range_apply(vv, 1, 4, bump_up, &bump);

		int arr_2[ASIZE] = {1, 12, 13, 14, 5};
		check(memcmp(vv->data, arr_2, ASIZE*sizeof(int)) == 0);
		
		vect_void_apply(vv, bump_up, &bump);
		
		int arr_3[ASIZE] = {11, 22, 23, 24, 15};
		check(memcmp(vv->data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_void_apply(vv, bump_first, &bump);
		
		int arr_4[ASIZE] = {21, 22, 23, 24, 15};
		check(memcmp(vv->data, arr_4, ASIZE*sizeof(int)) == 0);
		
		vect_void_destroy(vv);
	}
	
	{
		// vect_void_range_partition, vect_void_partition
		
		int arr[ASIZE] = {1, 2, 1, 3, 1};
		
		vect_void_create_arr(vv, sizeof(int), arr, ASIZE);
		check(memcmp(vv->data, arr, ASIZE*sizeof(int)) == 0);
		
		int buff;
		int key = 1;
		void * res = NULL;
		res = vect_void_range_partition(vv, 1, 4, pred_part, &key, &buff);
		check(res);
		check(3 == vect_void_distance(vv, res));
		
		int arr_2[ASIZE] = {1, 2, 3, 1, 1};
		check(memcmp(vv->data, arr_2, ASIZE*sizeof(int)) == 0);
		
		res = vect_void_partition(vv, pred_part, &key, &buff);
		check(res);
		check(2 == vect_void_distance(vv, res));
		
		int arr_3[ASIZE] = {2, 3, 1, 1, 1};
		check(memcmp(vv->data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_void_destroy(vv);
	}
	
	return true;
}

VECT_DEFINE(tint, int);

static bool test_vect_tint_create(void)
{
	vect_tint zero_, * zero = &zero_;
	memset(zero, 0, sizeof(*zero));
	
	vect_tint vv_, * vv = &vv_;
	memset(vv, 0, sizeof(*vv));
	check(memcmp(vv, zero, sizeof(*vv)) == 0);
	
	check(vect_tint_create(vv) == vv);
	
	vect_void * base = vect_tint_get_base(vv);
	check(&vv->vect == base);
	
	check(memcmp(vv, zero, sizeof(*vv)) != 0);
	check(vv->vect.data);
	check(0 == vv->vect.len);
	check(32 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	vect_tint_destroy(vv);
	check(memcmp(vv, zero, sizeof(*vv)) == 0);
	return true;
}

static bool test_vect_tint_create_arr(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_tint vv_, * vv = &vv_;
	check(vect_tint_create_arr(vv, arr, ASIZE) == vv);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	vect_tint_destroy(vv);
	return true;
}

static bool test_vect_tint_resize(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_tint vv_, * vv = &vv_;
	vect_tint_create_arr(vv, arr, ASIZE);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_tint_resize(vv, ASIZE*2) == vv);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE*2 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_tint_resize(vv, 3) == vv);
	check(vv->vect.data);
	check(3 == vv->vect.len);
	check(3 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, 3*sizeof(int)) == 0);
	
	vect_tint_destroy(vv);
	return true;
}

static bool test_vect_tint_shrink_to_fit(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_tint vv_, * vv = &vv_;
	vect_tint_create_arr(vv, arr, ASIZE);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_tint_resize(vv, ASIZE*2) == vv);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE*2 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_tint_shrink_to_fit(vv) == vv);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);

	vect_tint_reset(vv);
	check(0 == vv->vect.len);
	check(!vect_tint_shrink_to_fit(vv));
	check(vv->vect.data);
	check(0 == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	vect_tint_destroy(vv);
	return true;
}

static bool test_vect_tint_getters(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_tint vv_, * vv = &vv_;
	vect_tint_create_arr(vv, arr, ASIZE);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_tint_data(vv) == vv->vect.data);
	check(vect_tint_len(vv) == vv->vect.len);
	check(vect_tint_cap(vv) == vv->vect.cap);
	check(vect_tint_elem_size(vv) == vv->vect.elem_size);
	
	vect_tint_destroy(vv);
	return true;
}

static bool test_vect_tint_is_empty(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_tint vv_, * vv = &vv_;
	check(vect_tint_create(vv) == vv);
	check(vv->vect.data);
	check(0 == vv->vect.len);
	check(32 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	check(vect_tint_is_empty(vv));
	
	vect_tint_destroy(vv);
	
	vect_tint_create_arr(vv, arr, ASIZE);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	check(!vect_tint_is_empty(vv));
	
	vect_tint_destroy(vv);
	return true;
}

static bool test_vect_tint_last_get_set_len_zero_distance(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_tint vv_, * vv = &vv_;
	vect_tint_create_arr(vv, arr, ASIZE);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	check(vect_tint_last(vv) == 4);
	check(1 == *(vect_tint_get(vv, 0)));
	check(3 == *(vect_tint_get(vv, 2)));
	check(5 == *(vect_tint_get(vv, vect_tint_last(vv))));
	
	check(0 == vect_tint_distance(vv, vect_tint_get(vv, 0)));
	check(1 == vect_tint_distance(vv, vect_tint_get(vv, 1)));
	check(2 == vect_tint_distance(vv, vect_tint_get(vv, 2)));
	check(3 == vect_tint_distance(vv, vect_tint_get(vv, 3)));
	check(4 == vect_tint_distance(vv, vect_tint_get(vv, 4)));

	vect_tint_set_len(vv, 3);
	check(vv->vect.data);
	check(3 == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, 3*sizeof(int)) == 0);
	
	check(vect_tint_last(vv) == 2);
	check(1 == *(vect_tint_get(vv, 0)));
	check(3 == *(vect_tint_get(vv, 2)));
	check(3 == *(vect_tint_get(vv, vect_tint_last(vv))));
	
	int arr_zero_len[3] = {0, 0, 0};
	int arr_zero_cap[ASIZE] = {0, 0, 0, 0, 0};
	
	check(!memcmp(vv->vect.data, arr_zero_len, 3*sizeof(int)) == 0);
	check(!memcmp(vv->vect.data, arr_zero_cap, ASIZE*sizeof(int)) == 0);
	
	vect_tint_len_zero(vv);
	check(memcmp(vv->vect.data, arr_zero_len, 3*sizeof(int)) == 0);
	check(!memcmp(vv->vect.data, arr_zero_cap, ASIZE*sizeof(int)) == 0);
	
	vect_tint_cap_zero(vv);
	check(memcmp(vv->vect.data, arr_zero_len, 3*sizeof(int)) == 0);
	check(memcmp(vv->vect.data, arr_zero_cap, ASIZE*sizeof(int)) == 0);
	
	vect_tint_destroy(vv);
	return true;
}

static bool test_vect_tint_push_pop_peek_write_pop_n(void)
{
	int elem = 5;
	
	vect_tint vv_, * vv = &vv_;
	check(vect_tint_create_cap(vv, 1) == vv);
	
	check(vv->vect.data);
	check(0 == vv->vect.len);
	check(1 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	check(5 == *(vect_tint_push(vv, &elem)));
	check(5 == *(vect_tint_get(vv, 0)));
	check(vv->vect.data);
	check(1 == vv->vect.len);
	check(1 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	elem = 6;
	check(6 == *(vect_tint_write(vv, 0, &elem)));
	
	elem = 7;
	check(7 == *(vect_tint_push(vv, &elem)));
	check(7 == *(vect_tint_peek(vv)));
	check(vv->vect.data);
	check(2 == vv->vect.len);
	check(2 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	elem = 8;
	check(8 == *(vect_tint_write(vv, 1, &elem)));
	check(8 == *(vect_tint_peek(vv)));
	
	elem = 9;
	check(9 == *(vect_tint_push(vv, &elem)));
	check(9 == *(vect_tint_peek(vv)));
	check(vv->vect.data);
	check(3 == vv->vect.len);
	check(4 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);

	vect_tint_pop(vv);
	check(8 == *(vect_tint_peek(vv)));
	check(vv->vect.data);
	check(2 == vv->vect.len);
	check(4 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);

	check(9 == *(vect_tint_push(vv, &elem)));
	check(vv->vect.data);
	check(3 == vv->vect.len);
	check(4 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	check(9 == *(vect_tint_push(vv, &elem)));
	check(vv->vect.data);
	check(4 == vv->vect.len);
	check(4 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	check(9 == *(vect_tint_push(vv, &elem)));
	check(vv->vect.data);
	check(5 == vv->vect.len);
	check(8 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	vect_tint_pop_n(vv, 3);
	check(8 == *(vect_tint_peek(vv)));
	check(vv->vect.data);
	check(2 == vv->vect.len);
	check(8 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	vect_tint_destroy(vv);
	return true;
}

static bool test_vect_tint_swap_pop(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	vect_tint vv_, * vv = &vv_;
	vect_tint_create_arr(vv, arr, ASIZE);
	check(vv->vect.data);
	check(ASIZE == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
	
	vect_tint_swap_pop(vv, 2);
	
	int arr_1[ASIZE-1] = {1, 2, 5, 4};
	check(ASIZE-1 == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr_1, (ASIZE-1)*sizeof(int)) == 0);
	
	int arr_2[ASIZE-2] = {4, 2, 5};
	vect_tint_swap_pop(vv, 0);
	check(ASIZE-2 == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr_2, (ASIZE-2)*sizeof(int)) == 0);
	
	int arr_3[ASIZE-3] = {4, 2};
	vect_tint_swap_pop(vv, vect_tint_last(vv));
	check(ASIZE-3 == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr_3, (ASIZE-3)*sizeof(int)) == 0);
	
	int arr_4[ASIZE-4] = {2};
	vect_tint_swap_pop(vv, 0);
	check(ASIZE-4 == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(memcmp(vv->vect.data, arr_4, (ASIZE-4)*sizeof(int)) == 0);
	check(!vect_tint_is_empty(vv));
	
	vect_tint_swap_pop(vv, 0);
	check(0 == vv->vect.len);
	check(ASIZE == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	check(vect_tint_is_empty(vv));
	
	vect_tint_destroy(vv);
	return true;
}

static bool test_vect_tint_erase(void)
{	
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	vect_tint vv_, * vv = &vv_;
	
	{
		vect_tint_create_arr(vv, arr, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int arr_2[ASIZE-1] = {2, 3, 4, 5};
		vect_tint_erase(vv, 0);
		check(ASIZE-1 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_2, (ASIZE-1)*sizeof(int)) == 0);
		
		int arr_3[ASIZE-2] = {2, 3, 4};
		vect_tint_erase(vv, vect_tint_last(vv));
		check(ASIZE-2 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_3, (ASIZE-2)*sizeof(int)) == 0);
		
		int arr_4[ASIZE-3] = {2, 4};
		vect_tint_erase(vv, 1);
		check(ASIZE-3 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_4, (ASIZE-3)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
	}
	
	{
		vect_tint_create_arr(vv, arr, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase(vv, 0, 3);
		int arr_2[ASIZE-2] = {4, 5};
		check(ASIZE-3 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_2, (ASIZE-3)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
		
		vect_tint_create_arr(vv, arr, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase(vv, 3, vect_tint_len(vv));
		int arr_3[ASIZE-2] = {1, 2, 3};
		check(ASIZE-2 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_3, (ASIZE-2)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
		
		vect_tint_create_arr(vv, arr, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase(vv, 1, vect_tint_last(vv));
		int arr_4[ASIZE-2] = {1, 5};
		check(ASIZE-3 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_4, (ASIZE-3)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
	}
	
	{
		vect_tint_create_arr(vv, arr, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase(vv, 0, 0);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase(vv, 0, 1);
		int arr_2[ASIZE-1] = {2, 3, 4, 5};
		check(ASIZE-1 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_2, (ASIZE-1)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
		
		vect_tint_create_arr(vv, arr, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase(vv, vect_tint_last(vv), vect_tint_len(vv));
		int arr_3[ASIZE-1] = {1, 2, 3, 4};
		check(ASIZE-1 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_3, (ASIZE-1)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
		
		vect_tint_create_arr(vv, arr, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase(vv, 1, 2);
		int arr_4[ASIZE-1] = {1, 3, 4, 5};
		check(ASIZE-1 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(memcmp(vv->vect.data, arr_4, (ASIZE-3)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
	}
	
	{
		#undef ASIZE
		#define ASIZE 11
		
		int arr_1[ASIZE] = {0, 0, 1, 2, 0, 3, 0, 4, 5, 0, 0};
		
		vect_tint_create_arr(vv, arr_1, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr_1, ASIZE*sizeof(int)) == 0);
		
		size_t new_size = 5;
		int what = 0;
		vect_tint_erase_if(vv, erase_pred, &what);
		check(vv->vect.len == new_size);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr, new_size*sizeof(int)) == 0);
		vect_tint_destroy(vv);
		
		vect_tint_create_arr(vv, arr_1, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr_1, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase_if(vv, 1, 1, erase_pred, &what);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr_1, ASIZE*sizeof(int)) == 0);
		
		int arr_2[ASIZE-1] = {0, 1, 2, 0, 3, 0, 4, 5, 0, 0}; 
		vect_tint_range_erase_if(vv, 1, 2, erase_pred, &what);
		check(ASIZE-1 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr_2, (ASIZE-1)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
		
		vect_tint_create_arr(vv, arr_1, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr_1, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_erase_if(vv, 1, 4, erase_pred, &what);
		check(ASIZE-1 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr_2, (ASIZE-1)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
		
		vect_tint_create_arr(vv, arr_1, ASIZE);
		check(vv->vect.data);
		check(ASIZE == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr_1, ASIZE*sizeof(int)) == 0);
		
		int arr_3[ASIZE-2] = {0, 1, 2, 3, 0, 4, 5, 0, 0};
		vect_tint_range_erase_if(vv, 1, 5, erase_pred, &what);
		check(ASIZE-2 == vv->vect.len);
		check(ASIZE == vv->vect.cap);
		check(sizeof(int) == vv->vect.elem_size);
		check(memcmp(vv->vect.data, arr_3, (ASIZE-2)*sizeof(int)) == 0);
		vect_tint_destroy(vv);
		
		#undef ASIZE
		#define ASIZE 5
	}
	
	return true;
}

static bool test_vect_tint_insert_append_reset(void)
{
	
	vect_tint vv_, * vv = &vv_;
	check(vect_tint_create_cap(vv, 1) == vv);
	
	check(vv->vect.data);
	check(0 == vv->vect.len);
	check(1 == vv->vect.cap);
	check(sizeof(int) == vv->vect.elem_size);
	
	int elem = 5;
	check(5 == *(vect_tint_push(vv, &elem)));
	check(1 == vv->vect.len);
	check(1 == vv->vect.cap);
	
	elem = 3;
	check(3 == *(vect_tint_insert(vv, 0, &elem)));
	check(3 == *(vect_tint_get(vv, 0)));
	check(5 == *(vect_tint_get(vv, 1)));
	check(2 == vv->vect.len);
	check(2 == vv->vect.cap);
	
	elem = 4;
	check(4 == *(vect_tint_insert(vv, vect_tint_last(vv), &elem)));
	check(3 == *(vect_tint_get(vv, 0)));
	check(4 == *(vect_tint_get(vv, 1)));
	check(5 == *(vect_tint_get(vv, 2)));
	check(3 == vv->vect.len);
	check(4 == vv->vect.cap);	
	
	int ins[3] = {7, 8, 9};
	int arr_1[6] = {7, 8, 9, 3, 4, 5};
	
	check(vv == vect_tint_insert_n(vv, 0, ins, 3));
	check(6 == vv->vect.len);
	check(8 == vv->vect.cap);
	check(memcmp(vv->vect.data, arr_1, 6*sizeof(int)) == 0);
	
	int arr_2[8] = {7, 8, 9, 3, 4, 7, 8, 5};
	check(vv == vect_tint_insert_n(vv, vect_tint_last(vv), ins, 2));
	check(8 == vv->vect.len);
	check(8 == vv->vect.cap);
	check(memcmp(vv->vect.data, arr_2, 8*sizeof(int)) == 0);

	vect_tint_reset(vv);
	check(0 == vv->vect.len);
	check(8 == vv->vect.cap);
	
	check(vv == vect_tint_append(vv, ins, 2));
	check(2 == vv->vect.len);
	check(8 == vv->vect.cap);
	check(memcmp(vv->vect.data, ins, 2*sizeof(int)) == 0);
	
	int arr_3[5] = {7, 8, 7, 8, 9};
	check(vv == vect_tint_append(vv, ins, 3));
	check(5 == vv->vect.len);
	check(8 == vv->vect.cap);
	check(memcmp(vv->vect.data, arr_3, 5*sizeof(int)) == 0);

	int ins_2[3] = {1, 2, 3};
	int arr_4[7] = {7, 8, 1, 2, 7, 8, 9};
	check(vv == vect_tint_insert_n(vv, 2, ins_2, 2));
	check(7 == vv->vect.len);
	check(8 == vv->vect.cap);
	check(memcmp(vv->vect.data, arr_4, 7*sizeof(int)) == 0);
	
	int arr_5[10] = {7, 8, 1, 2, 7, 8, 9, 1, 2, 3};
	check(vv == vect_tint_append(vv, ins_2, 3));
	check(10 == vv->vect.len);
	check(16 == vv->vect.cap);
	check(memcmp(vv->vect.data, arr_5, 10*sizeof(int)) == 0);
	
	vect_tint_destroy(vv);
	
	check(vect_tint_create_cap(vv, 1) == vv);
	check(vv == vect_tint_append(vv, arr_5, 10));
	check(10 == vv->vect.len);
	check(16 == vv->vect.cap);
	check(memcmp(vv->vect.data, arr_5, 10*sizeof(int)) == 0);
	
	vect_tint_destroy(vv);
	
	return true;
}

static bool test_vect_tint_algo_wrappers(void)
{
	vect_tint vv_, * vv = &vv_;
	
	{
		// vect_tint_range_set_to, vect_tint_set_to
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int arr_2[ASIZE] = {1, 7, 7, 7, 5};
		int key = 7;
		vect_tint_range_set_to(vv, 1, vect_tint_last(vv), &key);
		check(memcmp(vv->vect.data, arr_2, ASIZE*sizeof(int)) == 0);
		
		int arr_3[ASIZE] = {8, 8, 8, 8, 8};
		key = 8;
		vect_tint_set_to(vv, &key);
		check(memcmp(vv->vect.data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_copy, vect_tint_copy
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		bool res = false;
		vect_tint vcp = vect_tint_range_copy(vv, 1, 4, &res);
		check(res);
		check(vcp.vect.data);
		check(vcp.vect.len == 3);
		check(vcp.vect.cap == 3);
		check(vcp.vect.elem_size == sizeof(int));
		check(memcmp(vcp.vect.data, arr+1, 3*sizeof(int)) == 0);
		
		vect_tint_destroy(&vcp);
		check(vcp.vect.data == NULL);
		check(vcp.vect.len == 0);
		check(vcp.vect.cap == 0);
		check(vcp.vect.elem_size == 0);
		
		res = false;
		vcp = vect_tint_copy(vv, &res);
		check(res);
		check(vcp.vect.data);
		check(vcp.vect.len == vv->vect.len);
		check(vcp.vect.cap == vv->vect.cap);
		check(vcp.vect.elem_size == vv->vect.elem_size);
		check(memcmp(vcp.vect.data, vv->vect.data, vv->vect.len*sizeof(int))
			== 0);
		
		vect_tint_destroy(&vcp);
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_is_sorted vect_tint_is_sorted
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		check(vect_tint_range_is_sorted(vv, 1, 1, compar));
		check(vect_tint_range_is_sorted(vv, 1, 4, compar));
		check(vect_tint_is_sorted(vv, compar));
		vect_tint_destroy(vv);
		
		int arr_2[ASIZE] = {1, 2, 3, 5, 4};
		vect_tint_create_arr(vv, arr_2, ASIZE);
		check(memcmp(vv->vect.data, arr_2, ASIZE*sizeof(int)) == 0);
		
		check(vect_tint_range_is_sorted(vv, 1, 4, compar));
		check(!vect_tint_range_is_sorted(vv, 1, 5, compar));
		check(!vect_tint_is_sorted(vv, compar));
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_sort, vect_tint_sort
		
		int arr[ASIZE] = {5, 4, 3, 2, 1};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		check(!vect_tint_range_is_sorted(vv, 1, 4, compar));
		vect_tint_range_sort(vv, 1, 4, compar);
		check(vect_tint_range_is_sorted(vv, 1, 4, compar));
		
		int arr_2[ASIZE] = {5, 2, 3, 4, 1};
		check(memcmp(vv->vect.data, arr_2, ASIZE*sizeof(int)) == 0);
		
		check(!vect_tint_is_sorted(vv, compar));
		vect_tint_sort(vv, compar);
		check(vect_tint_is_sorted(vv, compar));
		
		int arr_3[ASIZE] = {1, 2, 3, 4, 5};
		check(memcmp(vv->vect.data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_find, vect_tint_find
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 1;
		check(!vect_tint_range_find(vv, 1, 4, &key, compar));
		
		key = 3;
		int * res = vect_tint_range_find(vv, 1, 4, &key, compar);
		check(res);
		check(vect_tint_distance(vv, res) == 2);
		
		key = -1000;
		check(!vect_tint_find(vv, &key, compar));
		
		key = 1;
		res = vect_tint_find(vv, &key, compar);
		check(res);
		check(vect_tint_distance(vv, res) == 0);
		
		
		key = 5;
		res = vect_tint_find(vv, &key, compar);
		check(res);
		check(vect_tint_distance(vv, res) == 4);
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_bsearch, vect_tint_bsearch
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 1;
		check(!vect_tint_range_bsearch(vv, 1, 4, &key, compar));
		
		key = 3;
		const int * res = vect_tint_range_bsearch(vv, 1, 4, &key, compar);
		check(res);
		check(vect_tint_distance(vv, res) == 2);
		
		key = -1000;
		check(!vect_tint_bsearch(vv, &key, compar));
		
		key = 1;
		res = vect_tint_bsearch(vv, &key, compar);
		check(res);
		check(vect_tint_distance(vv, res) == 0);
		
		
		key = 5;
		res = vect_tint_bsearch(vv, &key, compar);
		check(res);
		check(vect_tint_distance(vv, res) == 4);
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_sfirst_eq, vect_tint_sfirst_eq
		
		int arr[ASIZE] = {1, 2, 2, 5, 6};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 5;
		check(!vect_tint_range_sfirst_eq(vv, 0, 3, &key, compar));
		
		key = 2;
		const int * res = vect_tint_range_sfirst_eq(vv, 0, 3, &key, compar);
		check(res);
		check(1 == vect_tint_distance(vv, res));
		
		res = vect_tint_sfirst_eq(vv, &key, compar);
		check(res);
		check(1 == vect_tint_distance(vv, res));
		
		key = 1;
		res = vect_tint_sfirst_eq(vv, &key, compar);
		check(res);
		check(0 == vect_tint_distance(vv, res));
		
		key = 5;
		res = vect_tint_sfirst_eq(vv, &key, compar);
		check(res);
		check(3 == vect_tint_distance(vv, res));
		
		key = 6;
		res = vect_tint_sfirst_eq(vv, &key, compar);
		check(res);
		check(4 == vect_tint_distance(vv, res));
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_sfirst_geq, vect_tint_range_geq
		
		int arr[ASIZE] = {1, 2, 2, 5, 1000};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 3;
		check(!vect_tint_range_sfirst_geq(vv, 0, 3, &key, compar));
		
		key = 2;
		const int * res = vect_tint_range_sfirst_geq(vv, 0, 3, &key, compar);
		check(res);
		check(1 == vect_tint_distance(vv, res));
		
		key = 1;
		res = vect_tint_range_sfirst_geq(vv, 0, 3, &key, compar);
		check(res);
		check(0 == vect_tint_distance(vv, res));
		
		key = -1000;
		res = vect_tint_range_sfirst_geq(vv, 0, 3, &key, compar);
		check(res);
		check(0 == vect_tint_distance(vv, res));
		
		key = 1001;
		check(!vect_tint_sfirst_geq(vv, &key, compar));
		
		key = 900;
		res = vect_tint_sfirst_geq(vv, &key, compar);
		check(res);
		check(4 == vect_tint_distance(vv, res));
		
		key = 0;
		res = vect_tint_sfirst_geq(vv, &key, compar);
		check(res);
		check(0 == vect_tint_distance(vv, res));
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_sfirst_gt, vect_tint_sfirst_gt
		
		int arr[ASIZE] = {1, 2, 2, 5, 1000};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 3;
		check(!vect_tint_range_sfirst_gt(vv, 0, 3, &key, compar));
		
		key = 1;
		const int * res = vect_tint_range_sfirst_gt(vv, 0, 3, &key, compar);
		check(res);
		check(1 == vect_tint_distance(vv, res));
		
		key = 1;
		res = vect_tint_range_sfirst_gt(vv, 0, 3, &key, compar);
		check(res);
		check(1 == vect_tint_distance(vv, res));
		
		key = -1000;
		res = vect_tint_range_sfirst_gt(vv, 0, 3, &key, compar);
		check(res);
		check(0 == vect_tint_distance(vv, res));
		
		key = 1001;
		check(!vect_tint_sfirst_gt(vv, &key, compar));
		
		key = 900;
		res = vect_tint_sfirst_gt(vv, &key, compar);
		check(res);
		check(4 == vect_tint_distance(vv, res));
		
		key = 0;
		res = vect_tint_sfirst_gt(vv, &key, compar);
		check(res);
		check(0 == vect_tint_distance(vv, res));
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_is_eq
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		bool is_ok = false;
		vect_tint cpy = vect_tint_copy(vv, &is_ok);
		check(is_ok);
		check(vect_tint_is_eq(vv, &cpy, compar));
		
		int key = 10;
		vect_tint_write(vv, 0, &key);
		check(!vect_tint_is_eq(vv, &cpy, compar));
		
		vect_tint_destroy(&cpy);
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_swap
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_swap(vv, 0, 0);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_swap(vv, 2, 2);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		check(1 == *(vect_tint_get(vv, 0)));
		check(5 == *(vect_tint_get(vv, vect_tint_last(vv))));
		
		vect_tint_swap(vv, 0, vect_tint_last(vv));
		
		check(5 == *(vect_tint_get(vv, 0)));
		check(1 == *(vect_tint_get(vv, vect_tint_last(vv))));
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_reverse, vect_tint_reverse
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		vect_tint_range_reverse(vv, 1, 4);
		
		int arr_2[ASIZE] = {1, 4, 3 ,2 ,5};
		check(memcmp(vv->vect.data, arr_2, ASIZE*sizeof(int)) == 0);
		
		int arr_3[ASIZE] = {5, 2, 3, 4, 1};
		vect_tint_reverse(vv);
		check(memcmp(vv->vect.data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_tint_pop(vv);
		vect_tint_reverse(vv);
		
		int arr_4[ASIZE-1] = {4, 3, 2, 5};
		check(memcmp(vv->vect.data, arr_4, (ASIZE-1)*sizeof(int)) == 0);
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_apply, vect_tint_apply
		
		int arr[ASIZE] = {1, 2, 3, 4, 5};
		
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int bump = 10;
		vect_tint_range_apply(vv, 1, 4, bump_up, &bump);

		int arr_2[ASIZE] = {1, 12, 13, 14, 5};
		check(memcmp(vv->vect.data, arr_2, ASIZE*sizeof(int)) == 0);
		
		vect_tint_apply(vv, bump_up, &bump);
		
		int arr_3[ASIZE] = {11, 22, 23, 24, 15};
		check(memcmp(vv->vect.data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_tint_apply(vv, bump_first, &bump);
		
		int arr_4[ASIZE] = {21, 22, 23, 24, 15};
		check(memcmp(vv->vect.data, arr_4, ASIZE*sizeof(int)) == 0);
		
		vect_tint_destroy(vv);
	}
	
	{
		// vect_tint_range_partition, vect_tint_partition
		
		int arr[ASIZE] = {1, 2, 1, 3, 1};
		
		vect_tint_create_arr(vv, arr, ASIZE);
		check(memcmp(vv->vect.data, arr, ASIZE*sizeof(int)) == 0);
		
		int key = 1;
		int * res = NULL;
		res = vect_tint_range_partition(vv, 1, 4, pred_part, &key);
		check(res);
		check(3 == vect_tint_distance(vv, res));
		
		int arr_2[ASIZE] = {1, 2, 3, 1, 1};
		check(memcmp(vv->vect.data, arr_2, ASIZE*sizeof(int)) == 0);
		
		res = vect_tint_partition(vv, pred_part, &key);
		check(res);
		check(2 == vect_tint_distance(vv, res));
		
		int arr_3[ASIZE] = {2, 3, 1, 1, 1};
		check(memcmp(vv->vect.data, arr_3, ASIZE*sizeof(int)) == 0);
		
		vect_tint_destroy(vv);
	}
	
	return true;
}

static ftest tests[] = {
	test_vect_void_create,
	test_vect_void_create_arr,
	test_vect_void_resize,
	test_vect_void_shrink_to_fit,
	test_vect_void_getters,
	test_vect_void_is_empty,
	test_vect_void_last_get_set_len_zero_distance,
	test_vect_void_push_pop_peek_write_pop_n,
	test_vect_void_swap_pop,
	test_vect_void_erase,
	test_vect_void_insert_append_reset,
	test_vect_void_algo_wrappers,
	test_vect_tint_create,
	test_vect_tint_create_arr,
	test_vect_tint_resize,
	test_vect_tint_shrink_to_fit,
	test_vect_tint_getters,
	test_vect_tint_is_empty,
	test_vect_tint_last_get_set_len_zero_distance,
	test_vect_tint_push_pop_peek_write_pop_n,
	test_vect_tint_swap_pop,
	test_vect_tint_erase,
	test_vect_tint_insert_append_reset,
	test_vect_tint_algo_wrappers,
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
