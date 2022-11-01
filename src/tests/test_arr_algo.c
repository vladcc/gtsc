#include <stdio.h>
#include <string.h>
#include "test.h"
#include "arr_algo/arr_algo.h"

#define UNUSED(x) (void)(x)

#define ASIZE 5

static void arr_print(int * arr, int size)
{
	if (size)
	{
		putchar('{');
		for (int i = 0; i < size; ++i)
			printf("%d, ", arr[i]);
		puts("}");
	}
}

static bool test_arr_void_info_init(void)
{
	arr_print(NULL, 0); // use arr_print(); keep gcc quiet
	
	arr_void_info ainf_, * ainf = &ainf_;
	memset(ainf, 0, sizeof(*ainf));

	check(NULL == ainf->data);
	check(0 == ainf->elem_size);
	check(0 == ainf->start);
	check(0 == ainf->end_non_incl);

	int arr[ASIZE] = {0};
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);

	check(arr == ainf->data);
	check(sizeof(int) == ainf->elem_size);
	check(0 == ainf->start);
	check(5 == ainf->end_non_incl);
	
	return true;
}

static bool test_arr_void_distance(void)
{
	int arr[ASIZE] = {0};

	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	check(3 == arr_void_distance(ainf, arr+3));

	arr_void_info_set(ainf, arr, sizeof(int), 4, ASIZE);
	check(3 == arr_void_distance(ainf, arr+3));

	arr_void_info_set(ainf, arr, sizeof(int), 0, 0);
	check(2 == arr_void_distance(ainf, arr+2));
	
	return true;
}

static bool test_arr_void_set_to(void)
{
	int fives[ASIZE] = {5, 5, 5, 5, 5};
	int arr[ASIZE] = {0, 0, 0, 0, 0};
	check(memcmp(arr, fives, ASIZE*sizeof(int)) != 0);

	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);

	int key = 5;
	arr_void_set_to(ainf, &key);
	check(memcmp(arr, fives, ASIZE*sizeof(int)) == 0);

	int res[ASIZE] = {5, 5, 4, 4, 5};
	arr_void_info_set(ainf, arr, sizeof(int), 2, 4);
	
	key = 4;
	arr_void_set_to(ainf, &key);
	check(memcmp(arr, res, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, 0);
	arr_void_set_to(ainf, &key);
	check(memcmp(arr, res, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 3, 3);
	arr_void_set_to(ainf, &key);
	check(memcmp(arr, res, ASIZE*sizeof(int)) == 0);

	int res2[ASIZE] = {4, 5, 4, 4, 5};
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	arr_void_set_to(ainf, &key);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);

	int res3[ASIZE] = {4, 5, 4, 4, 4};
	arr_void_info_set(ainf, arr, sizeof(int), 4, 5);
	arr_void_set_to(ainf, &key);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool test_arr_void_copy(void)
{
	int fives[ASIZE] = {5, 5, 5, 5, 5};
	int arr[ASIZE] = {0, 0, 0, 0, 0};
	check(memcmp(arr, fives, ASIZE*sizeof(int)) != 0);

	arr_void_info ainf_, * ainf = &ainf_;
	
	arr_void_info_set(ainf, fives, sizeof(int), 0, ASIZE);
	arr_void_copy(ainf, arr);
	check(memcmp(arr, fives, ASIZE*sizeof(int)) == 0);

	int zeroes[ASIZE] = {0, 0, 0, 0, 0};
	int arr2[ASIZE] = {0, 0, 0, 0, 0};
	int res1[ASIZE] = {0, 5, 0, 0, 0};
	int res2[ASIZE] = {0, 5, 5, 0, 0};
	check(memcmp(arr2, res1, ASIZE*sizeof(int)) != 0);
	check(memcmp(arr2, res2, ASIZE*sizeof(int)) != 0);
	check(memcmp(arr2, zeroes, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, fives, sizeof(int), 2, 2);
	arr_void_copy(ainf, arr2+1);
	check(memcmp(arr2, zeroes, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, fives, sizeof(int), 2, 3);
	arr_void_copy(ainf, arr2+1);
	check(memcmp(arr2, res1, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, fives, sizeof(int), 2, 4);
	arr_void_copy(ainf, arr2+1);
	check(memcmp(arr2, res2, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool test_arr_void_move(void)
{
	int fives[ASIZE] = {5, 5, 5, 5, 5};
	int arr[ASIZE] = {0, 0, 0, 0, 0};
	check(memcmp(arr, fives, ASIZE*sizeof(int)) != 0);

	arr_void_info ainf_, * ainf = &ainf_;
	
	arr_void_info_set(ainf, fives, sizeof(int), 0, ASIZE);
	arr_void_move(ainf, arr);
	check(memcmp(arr, fives, ASIZE*sizeof(int)) == 0);

	int zeroes[ASIZE] = {0, 0, 0, 0, 0};
	int arr2[ASIZE] = {0, 0, 0, 0, 0};
	int res1[ASIZE] = {0, 5, 0, 0, 0};
	int res2[ASIZE] = {0, 5, 5, 0, 0};
	check(memcmp(arr2, res1, ASIZE*sizeof(int)) != 0);
	check(memcmp(arr2, res2, ASIZE*sizeof(int)) != 0);
	check(memcmp(arr2, zeroes, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, fives, sizeof(int), 2, 2);
	arr_void_move(ainf, arr2+1);
	check(memcmp(arr2, zeroes, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, fives, sizeof(int), 2, 3);
	arr_void_move(ainf, arr2+1);
	check(memcmp(arr2, res1, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, fives, sizeof(int), 2, 4);
	arr_void_move(ainf, arr2+1);
	check(memcmp(arr2, res2, ASIZE*sizeof(int)) == 0);	

	{
		int arr[ASIZE] = {0, 0, 0, 4, 5};
		int res[ASIZE] = {0, 0, 4, 5, 5};
		check(memcmp(arr, res, ASIZE*sizeof(int)) != 0);
		
		arr_void_info_set(ainf, arr, sizeof(int), 3, 5);
		arr_void_move(ainf, arr+2);
		check(memcmp(arr, res, ASIZE*sizeof(int)) == 0);

		int res2[ASIZE] = {5, 0, 4, 5, 5};
		arr_void_info_set(ainf, arr, sizeof(int), 3, 4);
		arr_void_move(ainf, arr);
		check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	}
	
	return true;
}

static int compar(const void * key1, const void * key2)
{
	int a = *((const int *)key1);
	int b = *((const int *)key2);

	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

static bool test_arr_void_is_sorted(void)
{
	int arr[ASIZE] = {7, 4, 5, 5, 2};

	arr_void_info ainf_, * ainf = &ainf_;
	
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	check(!arr_void_is_sorted(ainf, compar));

	arr_void_info_set(ainf, arr, sizeof(int), 1, ASIZE);
	check(!arr_void_is_sorted(ainf, compar));

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE-1);
	check(!arr_void_is_sorted(ainf, compar));

	arr_void_info_set(ainf, arr, sizeof(int), 1, 4);
	check(arr_void_is_sorted(ainf, compar));

	arr_void_info_set(ainf, arr, sizeof(int), 1, 2);
	check(arr_void_is_sorted(ainf, compar));

	arr_void_info_set(ainf, arr, sizeof(int), 1, 1);
	check(arr_void_is_sorted(ainf, compar));
	
	return true;
}

static bool test_arr_void_sort(void)
{
	int arr[ASIZE] = {5, 4, 3, 2, 1};
	int res0[ASIZE] = {5, 4, 3, 2, 1};
	int res1[ASIZE] = {5, 3, 4, 2, 1};
	int res2[ASIZE] = {5, 2, 3, 4, 1};
	int res3[ASIZE] = {1, 2, 3, 4, 5};

	check(memcmp(arr, res0, ASIZE*sizeof(int)) == 0);

	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 1, 1);
	
	arr_void_sort(ainf, compar);
	check(memcmp(arr, res0, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 1, 2);
	arr_void_sort(ainf, compar);
	check(memcmp(arr, res0, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 1, 2);
	arr_void_sort(ainf, compar);
	check(memcmp(arr, res0, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 1, 3);
	arr_void_sort(ainf, compar);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 1, 3);
	arr_void_sort(ainf, compar);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 1, 4);
	arr_void_sort(ainf, compar);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	arr_void_sort(ainf, compar);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool part_pred(const void * key, void * arg)
{
	return *((int *)key) == *((int *)arg);
}
static bool part_pred2(const void * key, void * arg)
{
	return *((int *)key) > *((int *)arg);
}
static bool test_arr_void_partition(void)
{
	#undef ASIZE
	#define ASIZE 10
	
	int tmpl[ASIZE] = {2, 1, 1, 3, 1, 1, 50, 20, 30, 1};
	int arr[ASIZE] = {2, 1, 1, 3, 1, 1, 50, 20, 30, 1};
	int res1[ASIZE] = {1, 2, 1, 3, 1, 1, 50, 20, 30, 1};
	int res2[ASIZE] = {1, 2, 1, 1, 1, 3, 50, 20, 30, 1};
	int res3[ASIZE] = {1, 1, 1, 1, 1, 3, 50, 20, 30, 2};
	int res4[ASIZE] = {3, 50, 20, 30, 1, 1, 1, 1, 1, 2};
	
	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 0, 0);
	
	int what = 1, buff;
	void * part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(!part);
	check(memcmp(arr, tmpl, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(part);
	check(arr_void_distance(ainf, part) == 0);
	check(memcmp(arr, tmpl, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 1, 2);
	part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(!part);
	check(memcmp(arr, tmpl, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 0, 2);
	part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(part);
	check(arr_void_distance(ainf, part) == 1);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 0, 2);
	part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(part);
	check(arr_void_distance(ainf, part) == 1);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 2, 6);
	part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(part);
	check(arr_void_distance(ainf, part) == 5);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(part);
	check(arr_void_distance(ainf, part) == 5);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	what = 1000;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(part);
	check(arr_void_distance(ainf, part) == 0);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	arr_void_info_set(ainf, arr, sizeof(int), 3, ASIZE);
	part = arr_void_partition(ainf, part_pred, &what, &buff);
	check(part);
	check(arr_void_distance(ainf, part) == 3);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	what = 2;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	part = arr_void_partition(ainf, part_pred2, &what, &buff);
	check(part);
	check(arr_void_distance(ainf, part) == 4);
	check(memcmp(arr, res4, ASIZE*sizeof(int)) == 0);
	
	return true;
	
	#undef ASIZE
	#define ASIZE 5
}

static bool test_arr_void_find(void)
{
	int arr[ASIZE] = {1, 2, 3, 1, 4};

	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 1, 1);

	int key = 1;
	void * found = arr_void_find(ainf, &key, compar);
	check(!found);

	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_find(ainf, &key, compar);
	check(found);
	check((void*)arr == found);
	check(0 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 1, 3);
	found = arr_void_find(ainf, &key, compar);
	check(!found);

	arr_void_info_set(ainf, arr, sizeof(int), 1, 4);
	found = arr_void_find(ainf, &key, compar);
	check(found);
	check((void*)(arr+3) == found);
	check(3 == arr_void_distance(ainf, found));

	key = 2;
	arr_void_info_set(ainf, arr, sizeof(int), 2, ASIZE);
	found = arr_void_find(ainf, &key, compar);
	check(!found);

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_find(ainf, &key, compar);
	check(found);
	check(*(int*)found == key);
	check((void*)(arr+arr_void_distance(ainf, found)) == found);

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_find(ainf, &key, compar);
	check(*(int*)found == key);
	check(found);
	check((void*)(arr+arr_void_distance(ainf, found)) == found);
	
	key = 4;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_find(ainf, &key, compar);
	check(*(int*)found == key);
	check(found);
	check((ASIZE-1) == arr_void_distance(ainf, found));

	key = -123;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_find(ainf, &key, compar);
	check(!found);

	key = 10000;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_find(ainf, &key, compar);
	check(!found);

	key = 0;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_find(ainf, &key, compar);
	check(!found);
	
	return true;
}

static bool test_arr_void_bsearch(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};

	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 1, 1);

	int key = 2;
	void * found = arr_void_bsearch(ainf, &key, compar);
	check(!found);

	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_bsearch(ainf, &key, compar);
	check(!found);

	arr_void_info_set(ainf, arr, sizeof(int), 1, 2);
	found = arr_void_bsearch(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(1 == arr_void_distance(ainf, found));
	check(found == (void*)(arr+1));

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_bsearch(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(1 == arr_void_distance(ainf, found));

	key = 4;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_bsearch(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(3 == arr_void_distance(ainf, found));

	key = 5;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_bsearch(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(4 == arr_void_distance(ainf, found));

	key = 1;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_bsearch(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(0 == arr_void_distance(ainf, found));

	key = -5;
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_bsearch(ainf, &key, compar);
	check(!found);

	key = 0;
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_bsearch(ainf, &key, compar);
	check(!found);

	key = 10;
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_bsearch(ainf, &key, compar);
	check(!found);
	
	return true;
}

static bool test_arr_void_sfirst_eq(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};

	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 1, 1);

	int key = 2;
	void * found = arr_void_sfirst_eq(ainf, &key, compar);
	check(!found);

	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(!found);

	arr_void_info_set(ainf, arr, sizeof(int), 1, 2);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(1 == arr_void_distance(ainf, found));
	check(found == (void*)(arr+1));

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(1 == arr_void_distance(ainf, found));

	key = 4;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(3 == arr_void_distance(ainf, found));

	key = 5;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(4 == arr_void_distance(ainf, found));

	key = 1;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*((int*)found) = key);
	check(0 == arr_void_distance(ainf, found));

	key = -5;
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(!found);

	key = 0;
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(!found);

	key = 10;
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	found = arr_void_sfirst_eq(ainf, &key, compar);
	check(!found);
	
	return true;
}

static bool test_arr_void_sfirst_geq(void)
{
#undef ASIZE
#define ASIZE 7

	{
		int arr[2] = {1, 2};

		arr_void_info ainf_, * ainf = &ainf_;
		arr_void_info_set(ainf, arr, sizeof(int), 0, 2);

		int key = 2;
		void * found = arr_void_sfirst_geq(ainf, &key, compar);
		check(found);
		check(*((int*)found) == key);
		check(1 == arr_void_distance(ainf, found));

		key = 1;
		found = arr_void_sfirst_geq(ainf, &key, compar);
		check(found);
		check(*((int*)found) == key);
		check(0 == arr_void_distance(ainf, found));

		key = 0;
		found = arr_void_sfirst_geq(ainf, &key, compar);
		check(found);
		check(*((int*)found) == 1);
		check(0 == arr_void_distance(ainf, found));

		key = 3;
		found = arr_void_sfirst_geq(ainf, &key, compar);
		check(!found);
	}
	
	int arr[ASIZE] = {1, 2, 2, 3, 4, 4, 5};
	
	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);

	int key = 2;
	void * found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == key);
	check(1 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 1, ASIZE);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == key);
	check(1 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 2, ASIZE);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == key);
	check(2 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 4, ASIZE);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 4);
	check(4 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 1, 2);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == key);
	check(1 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 1, 1);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(!found);

	key = 4;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 4);
	check(4 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE-1);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 4);
	check(4 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE-2);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 4);
	check(4 == arr_void_distance(ainf, found));

	key = 5;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == key);
	check(6 == arr_void_distance(ainf, found));

	key = 5;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE-1);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(!found);

	key = 1000;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(!found);

	key = -10;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 1);
	check(0 == arr_void_distance(ainf, found));
	
	return true;
	
#undef ASIZE
#define ASIZE 5
}

static bool test_arr_void_sfirst_gt(void)
{
#undef ASIZE
#define ASIZE 7

	{
		int arr[2] = {1, 2};

		arr_void_info ainf_, * ainf = &ainf_;
		arr_void_info_set(ainf, arr, sizeof(int), 0, 2);

		int key = 2;
		void * found = arr_void_sfirst_gt(ainf, &key, compar);
		check(!found);

		key = 1;
		found = arr_void_sfirst_gt(ainf, &key, compar);
		check(found);
		check(*((int*)found) == 2);
		check(1 == arr_void_distance(ainf, found));

		key = 0;
		found = arr_void_sfirst_gt(ainf, &key, compar);
		check(found);
		check(*((int*)found) == 1);
		check(0 == arr_void_distance(ainf, found));

		key = 3;
		found = arr_void_sfirst_gt(ainf, &key, compar);
		check(!found);
	}
	
	int arr[ASIZE] = {1, 2, 2, 3, 4, 4, 5};
	
	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);

	int key = 2;
	void * found = arr_void_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 3);
	check(3 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 1, ASIZE);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 3);
	check(3 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 2, ASIZE);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 3);
	check(3 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 4, ASIZE);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 4);
	check(4 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 1, 2);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(!found);

	arr_void_info_set(ainf, arr, sizeof(int), 1, 1);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(!found);

	key = 4;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 5);
	check(6 == arr_void_distance(ainf, found));

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE-1);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(!found);
	
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE-2);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(!found);
	
	key = 5;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(!found);
	
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE-1);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(!found);

	key = 1000;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(!found);

	key = -10;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	found = arr_void_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*((int*)found) == 1);
	check(0 == arr_void_distance(ainf, found));
	
	return true;
	
#undef ASIZE
#define ASIZE 5
}

static bool test_arr_void_swap(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	int res1[ASIZE] = {1, 2, 3, 4, 5};
	int res2[ASIZE] = {5, 2, 3, 4, 1};
	int res3[ASIZE] = {1, 4, 3, 2, 5};
	
	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);

	int buff;
	arr_void_swap(ainf, 0, 0, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	arr_void_swap(ainf, 2, 2, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_swap(ainf, 0, 4, &buff);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	arr_void_swap(ainf, 0, 4, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	// the range should be ignored
	arr_void_info_set(ainf, arr, sizeof(int), 4, ASIZE);
	arr_void_swap(ainf, 0, 4, &buff);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	arr_void_swap(ainf, 0, 4, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	arr_void_swap(ainf, 4, 0, &buff);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	arr_void_swap(ainf, 4, 0, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_swap(ainf, 1, 3, &buff);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	arr_void_swap(ainf, 3, 1, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool test_arr_void_reverse(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	int res1[ASIZE] = {1, 2, 3, 4, 5};
	int res2[ASIZE] = {1, 2, 4, 3, 5};
	int res3[ASIZE] = {5, 4, 3, 2, 1};
	int res4[ASIZE] = {1, 2, 5, 4, 3};
	int res5[ASIZE] = {2, 1, 3, 4, 5};
	
	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 2, 2);

	int buff;
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 2, 3);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 2, 4);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 2, 4);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 2, ASIZE);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res4, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 2, ASIZE);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, 2);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res5, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, 2);
	arr_void_reverse(ainf, &buff);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool accumulate(void * key, void * arg)
{
	UNUSED(arg);
	int * acc = (int *)arg;
	*acc += *((int *)key);
	return true;
}
static bool incr(void * key, void * arg)
{
	UNUSED(arg);
	int * k = (int *)key;
	++*k;
	return true;
}
static bool decr(void * key, void * arg)
{
	UNUSED(arg);
	int * k = (int *)key;
	--*k;
	return true;
}
typedef struct foo {int a, b; bool found;} foo;
static bool myfind(void * key, void * arg)
{
	foo * pf = (foo *)arg;
	
	if (*((int *)key) == pf->a)
	{
		pf->found = true;
		return false;
	}
	++pf->b;
	return true;
}
static bool test_arr_void_apply(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	int res1[ASIZE] = {1, 2, 3, 4, 5};
	int res2[ASIZE] = {2, 3, 4, 5, 6};
	
	arr_void_info ainf_, * ainf = &ainf_;
	arr_void_info_set(ainf, arr, sizeof(int), 0, 0);

	int out = 0;
	check(0 == out);
	arr_void_apply(ainf, accumulate, &out);
	check(0 == out);

	out = 0;
	check(0 == out);
	arr_void_info_set(ainf, arr, sizeof(int), 0, 1);
	arr_void_apply(ainf, accumulate, &out);
	check(1 == out);

	out = 0;
	check(0 == out);
	arr_void_info_set(ainf, arr, sizeof(int), 0, 2);
	arr_void_apply(ainf, accumulate, &out);
	check(3 == out);

	out = 0;
	check(0 == out);
	arr_void_info_set(ainf, arr, sizeof(int), 0, 2);
	arr_void_apply(ainf, accumulate, &out);
	check(3 == out);

	out = 0;
	check(0 == out);
	arr_void_info_set(ainf, arr, sizeof(int), 4, ASIZE);
	arr_void_apply(ainf, accumulate, &out);
	check(5 == out);

	out = 0;
	check(0 == out);
	arr_void_info_set(ainf, arr, sizeof(int), 3, ASIZE);
	arr_void_apply(ainf, accumulate, &out);
	check(9 == out);

	out = 0;
	check(0 == out);
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	arr_void_apply(ainf, accumulate, &out);
	check(15 == out);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	arr_void_apply(ainf, incr, NULL);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);

	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	arr_void_apply(ainf, decr, NULL);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	foo pf_ = {3, 0, false}, * pf = &pf_;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	arr_void_apply(ainf, myfind, pf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	check(3 == pf->a && 2 == pf->b && pf->found);

	memset(pf, 0, sizeof(*pf));
	check(0 == pf->a && 0 == pf->b && !pf->found);
	pf->a = 2;
	arr_void_info_set(ainf, arr, sizeof(int), 0, ASIZE);
	arr_void_apply(ainf, myfind, pf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	check(2 == pf->a && 1 == pf->b && pf->found);

	memset(pf, 0, sizeof(*pf));
	check(0 == pf->a && 0 == pf->b && !pf->found);
	pf->a = 2;
	arr_void_info_set(ainf, arr, sizeof(int), 2, ASIZE);
	arr_void_apply(ainf, myfind, pf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	check(2 == pf->a && 3 == pf->b && !pf->found);
	
	return true;
}

static bool test_arr_void_get(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	check(1 == *((int *)arr_void_get(arr, 0, sizeof(*arr))));
	check(2 == *((int *)arr_void_get(arr, 1, sizeof(*arr))));
	check(3 == *((int *)arr_void_get(arr, 2, sizeof(*arr))));
	check(4 == *((int *)arr_void_get(arr, 3, sizeof(*arr))));
	check(5 == *((int *)arr_void_get(arr, 4, sizeof(*arr))));
	return true;
}

static bool test_arr_void_write(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	int elem = 10;
	check(10 == *((int *)arr_void_write(arr, 0, sizeof(*arr), &elem)));

	elem = 20;
	check(20 == *((int *)arr_void_write(arr, 1, sizeof(*arr), &elem)));
	
	elem = 30;
	check(30 == *((int *)arr_void_write(arr, 2, sizeof(*arr), &elem)));
	
	elem = 40;
	check(40 == *((int *)arr_void_write(arr, 3, sizeof(*arr), &elem)));
	
	elem = 50;
	check(50 == *((int *)arr_void_write(arr, 4, sizeof(*arr), &elem)));
	return true;
}

static bool test_arr_void_is_eq(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	int arr_void_same[ASIZE] = {1, 2, 3, 4, 5};
	int arr_void_diff[ASIZE] = {0, 2, 3, 4, 0};
	unsigned char arr_void_b[ASIZE] = {1, 2, 3, 4, 5};
	
	arr_void_info arr_void_inf_, * arr_void_inf = &arr_void_inf_;
	arr_void_info_set(arr_void_inf, arr, sizeof(int), 0, ASIZE);
	
	arr_void_info arr_void_b_inf_, * arr_void_b_inf = &arr_void_b_inf_;
	arr_void_info_set(arr_void_b_inf, arr_void_b, sizeof(*arr_void_b), 0, ASIZE);
	
	check(!arr_void_is_eq(arr_void_inf, arr_void_b_inf, compar));
	
	arr_void_info arr_void_same_inf_, * arr_void_same_inf = &arr_void_same_inf_;
	arr_void_info_set(arr_void_same_inf, arr_void_same, sizeof(int), 0, ASIZE);
	
	check(arr_void_is_eq(arr_void_inf, arr_void_same_inf, compar));
	
	arr_void_info arr_void_diff_inf_, * arr_void_diff_inf = &arr_void_diff_inf_;
	arr_void_info_set(arr_void_diff_inf, arr_void_diff, sizeof(int), 0, ASIZE);
	
	check(!arr_void_is_eq(arr_void_inf, arr_void_diff_inf, compar));
	
	arr_void_info_set(arr_void_diff_inf, arr_void_diff, sizeof(int), 1, 4);
	arr_void_info_set(arr_void_inf, arr, sizeof(int), 1, 4);
	check(arr_void_is_eq(arr_void_inf, arr_void_diff_inf, compar));
	
	arr_void_info_set(arr_void_diff_inf, arr_void_diff, sizeof(int), 0, 0);
	arr_void_info_set(arr_void_b_inf, arr_void_b, sizeof(*arr_void_b), 0, 0);
	check(!arr_void_is_eq(arr_void_inf, arr_void_b_inf, compar));
	
	arr_void_info_set(arr_void_diff_inf, arr_void_diff, sizeof(int), 0, 0);
	arr_void_info_set(arr_void_inf, arr, sizeof(int), 0, 0);
	check(arr_void_is_eq(arr_void_inf, arr_void_diff_inf, compar));
	
	return true;
}

// (tint, int) will confirm the name and ret type are correctly macroed
ARR_ALGO_DEFINE(tint, int);

static bool test_arr_tint_info_init(void)
{
	arr_print(NULL, 0); // use arr_print(); keep gcc quiet
	
	arr_tint_info ainf_, * ainf = &ainf_;
	memset(ainf, 0, sizeof(*ainf));

	check(NULL == ainf->info.data);
	check(0 == ainf->info.elem_size);
	check(0 == ainf->info.start);
	check(0 == ainf->info.end_non_incl);

	int arr[ASIZE] = {0};
	arr_tint_info_set(ainf, arr, 0, ASIZE);

	check(arr == ainf->info.data);
	check(sizeof(int) == ainf->info.elem_size);
	check(0 == ainf->info.start);
	check(5 == ainf->info.end_non_incl);
	
	return true;
}

static bool test_arr_tint_result_diff(void)
{
	int arr[ASIZE] = {0};

	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	check(3 == arr_tint_distance(ainf, arr+3));

	arr_tint_info_set(ainf, arr, 4, ASIZE);
	check(3 == arr_tint_distance(ainf, arr+3));

	arr_tint_info_set(ainf, arr, 0, 0);
	check(2 == arr_tint_distance(ainf, arr+2));
	
	return true;
}

static bool test_arr_tint_set_to(void)
{
	int fives[ASIZE] = {5, 5, 5, 5, 5};
	int arr[ASIZE] = {0, 0, 0, 0, 0};
	check(memcmp(arr, fives, ASIZE*sizeof(int)) != 0);

	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 0, ASIZE);

	int key = 5;
	arr_tint_set_to(ainf, &key);
	check(memcmp(arr, fives, ASIZE*sizeof(int)) == 0);

	int res[ASIZE] = {5, 5, 4, 4, 5};
	arr_tint_info_set(ainf, arr, 2, 4);
	
	key = 4;
	arr_tint_set_to(ainf, &key);
	check(memcmp(arr, res, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, 0);
	arr_tint_set_to(ainf, &key);
	check(memcmp(arr, res, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 3, 3);
	arr_tint_set_to(ainf, &key);
	check(memcmp(arr, res, ASIZE*sizeof(int)) == 0);

	int res2[ASIZE] = {4, 5, 4, 4, 5};
	arr_tint_info_set(ainf, arr, 0, 1);
	arr_tint_set_to(ainf, &key);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);

	int res3[ASIZE] = {4, 5, 4, 4, 4};
	arr_tint_info_set(ainf, arr, 4, 5);
	arr_tint_set_to(ainf, &key);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool test_arr_tint_copy(void)
{
	int fives[ASIZE] = {5, 5, 5, 5, 5};
	int arr[ASIZE] = {0, 0, 0, 0, 0};
	check(memcmp(arr, fives, ASIZE*sizeof(int)) != 0);

	arr_tint_info ainf_, * ainf = &ainf_;
	
	arr_tint_info_set(ainf, fives, 0, ASIZE);
	arr_tint_copy(ainf, arr);
	check(memcmp(arr, fives, ASIZE*sizeof(int)) == 0);

	int zeroes[ASIZE] = {0, 0, 0, 0, 0};
	int arr2[ASIZE] = {0, 0, 0, 0, 0};
	int res1[ASIZE] = {0, 5, 0, 0, 0};
	int res2[ASIZE] = {0, 5, 5, 0, 0};
	check(memcmp(arr2, res1, ASIZE*sizeof(int)) != 0);
	check(memcmp(arr2, res2, ASIZE*sizeof(int)) != 0);
	check(memcmp(arr2, zeroes, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, fives, 2, 2);
	arr_tint_copy(ainf, arr2+1);
	check(memcmp(arr2, zeroes, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, fives, 2, 3);
	arr_tint_copy(ainf, arr2+1);
	check(memcmp(arr2, res1, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, fives, 2, 4);
	arr_tint_copy(ainf, arr2+1);
	check(memcmp(arr2, res2, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool test_arr_tint_move(void)
{
	int fives[ASIZE] = {5, 5, 5, 5, 5};
	int arr[ASIZE] = {0, 0, 0, 0, 0};
	check(memcmp(arr, fives, ASIZE*sizeof(int)) != 0);

	arr_tint_info ainf_, * ainf = &ainf_;
	
	arr_tint_info_set(ainf, fives, 0, ASIZE);
	arr_tint_move(ainf, arr);
	check(memcmp(arr, fives, ASIZE*sizeof(int)) == 0);

	int zeroes[ASIZE] = {0, 0, 0, 0, 0};
	int arr2[ASIZE] = {0, 0, 0, 0, 0};
	int res1[ASIZE] = {0, 5, 0, 0, 0};
	int res2[ASIZE] = {0, 5, 5, 0, 0};
	check(memcmp(arr2, res1, ASIZE*sizeof(int)) != 0);
	check(memcmp(arr2, res2, ASIZE*sizeof(int)) != 0);
	check(memcmp(arr2, zeroes, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, fives, 2, 2);
	arr_tint_move(ainf, arr2+1);
	check(memcmp(arr2, zeroes, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, fives, 2, 3);
	arr_tint_move(ainf, arr2+1);
	check(memcmp(arr2, res1, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, fives, 2, 4);
	arr_tint_move(ainf, arr2+1);
	check(memcmp(arr2, res2, ASIZE*sizeof(int)) == 0);	

	{
		int arr[ASIZE] = {0, 0, 0, 4, 5};
		int res[ASIZE] = {0, 0, 4, 5, 5};
		check(memcmp(arr, res, ASIZE*sizeof(int)) != 0);
		
		arr_tint_info_set(ainf, arr, 3, 5);
		arr_tint_move(ainf, arr+2);
		check(memcmp(arr, res, ASIZE*sizeof(int)) == 0);

		int res2[ASIZE] = {5, 0, 4, 5, 5};
		arr_tint_info_set(ainf, arr, 3, 4);
		arr_tint_move(ainf, arr);
		check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	}
	
	return true;
}

static bool test_arr_tint_is_sorted(void)
{
	int arr[ASIZE] = {7, 4, 5, 5, 2};

	arr_tint_info ainf_, * ainf = &ainf_;
	
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	check(!arr_tint_is_sorted(ainf, compar));

	arr_tint_info_set(ainf, arr, 1, ASIZE);
	check(!arr_tint_is_sorted(ainf, compar));

	arr_tint_info_set(ainf, arr, 0, ASIZE-1);
	check(!arr_tint_is_sorted(ainf, compar));

	arr_tint_info_set(ainf, arr, 1, 4);
	check(arr_tint_is_sorted(ainf, compar));

	arr_tint_info_set(ainf, arr, 1, 2);
	check(arr_tint_is_sorted(ainf, compar));

	arr_tint_info_set(ainf, arr, 1, 1);
	check(arr_tint_is_sorted(ainf, compar));
	
	return true;
}

static bool test_arr_tint_sort(void)
{
	int arr[ASIZE] = {5, 4, 3, 2, 1};
	int res0[ASIZE] = {5, 4, 3, 2, 1};
	int res1[ASIZE] = {5, 3, 4, 2, 1};
	int res2[ASIZE] = {5, 2, 3, 4, 1};
	int res3[ASIZE] = {1, 2, 3, 4, 5};

	check(memcmp(arr, res0, ASIZE*sizeof(int)) == 0);

	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 1, 1);
	
	arr_tint_sort(ainf, compar);
	check(memcmp(arr, res0, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 1, 2);
	arr_tint_sort(ainf, compar);
	check(memcmp(arr, res0, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 1, 2);
	arr_tint_sort(ainf, compar);
	check(memcmp(arr, res0, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 1, 3);
	arr_tint_sort(ainf, compar);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 1, 3);
	arr_tint_sort(ainf, compar);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 1, 4);
	arr_tint_sort(ainf, compar);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	arr_tint_sort(ainf, compar);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool test_arr_tint_partition(void)
{
	#undef ASIZE
	#define ASIZE 10
	
	int tmpl[ASIZE] = {2, 1, 1, 3, 1, 1, 50, 20, 30, 1};
	int arr[ASIZE] = {2, 1, 1, 3, 1, 1, 50, 20, 30, 1};
	int res1[ASIZE] = {1, 2, 1, 3, 1, 1, 50, 20, 30, 1};
	int res2[ASIZE] = {1, 2, 1, 1, 1, 3, 50, 20, 30, 1};
	int res3[ASIZE] = {1, 1, 1, 1, 1, 3, 50, 20, 30, 2};
	int res4[ASIZE] = {3, 50, 20, 30, 1, 1, 1, 1, 1, 2};
	
	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 0, 0);
	
	int what = 1;
	int * part = arr_tint_partition(ainf, part_pred, &what);
	check(!part);
	check(memcmp(arr, tmpl, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 0, 1);
	part = arr_tint_partition(ainf, part_pred, &what);
	check(part);
	check(arr_tint_distance(ainf, part) == 0);
	check(memcmp(arr, tmpl, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 1, 2);
	part = arr_tint_partition(ainf, part_pred, &what);
	check(!part);
	check(memcmp(arr, tmpl, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 0, 2);
	part = arr_tint_partition(ainf, part_pred, &what);
	check(part);
	check(arr_tint_distance(ainf, part) == 1);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 0, 2);
	part = arr_tint_partition(ainf, part_pred, &what);
	check(part);
	check(arr_tint_distance(ainf, part) == 1);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 2, 6);
	part = arr_tint_partition(ainf, part_pred, &what);
	check(part);
	check(arr_tint_distance(ainf, part) == 5);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	part = arr_tint_partition(ainf, part_pred, &what);
	check(part);
	check(arr_tint_distance(ainf, part) == 5);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	what = 1000;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	part = arr_tint_partition(ainf, part_pred, &what);
	check(part);
	check(arr_tint_distance(ainf, part) == 0);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	arr_tint_info_set(ainf, arr, 3, ASIZE);
	part = arr_tint_partition(ainf, part_pred, &what);
	check(part);
	check(arr_tint_distance(ainf, part) == 3);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	
	what = 2;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	part = arr_tint_partition(ainf, part_pred2, &what);
	check(part);
	check(arr_tint_distance(ainf, part) == 4);
	check(memcmp(arr, res4, ASIZE*sizeof(int)) == 0);
	
	return true;
	
	#undef ASIZE
	#define ASIZE 5
}

static bool test_arr_tint_find(void)
{
	int arr[ASIZE] = {1, 2, 3, 1, 4};

	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 1, 1);

	int key = 1;
	int * found = arr_tint_find(ainf, &key, compar);
	check(!found);

	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_find(ainf, &key, compar);
	check(found);
	check(arr == found);
	check(0 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 1, 3);
	found = arr_tint_find(ainf, &key, compar);
	check(!found);

	arr_tint_info_set(ainf, arr, 1, 4);
	found = arr_tint_find(ainf, &key, compar);
	check(found);
	check((arr+3) == found);
	check(3 == arr_tint_distance(ainf, found));

	key = 2;
	arr_tint_info_set(ainf, arr, 2, ASIZE);
	found = arr_tint_find(ainf, &key, compar);
	check(!found);

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_find(ainf, &key, compar);
	check(found);
	check(*(int*)found == key);
	check((arr+arr_tint_distance(ainf, found)) == found);

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_find(ainf, &key, compar);
	check(*(int*)found == key);
	check(found);
	check((arr+arr_tint_distance(ainf, found)) == found);
	
	key = 4;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_find(ainf, &key, compar);
	check(*(int*)found == key);
	check(found);
	check((ASIZE-1) == arr_tint_distance(ainf, found));

	key = -123;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_find(ainf, &key, compar);
	check(!found);

	key = 10000;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_find(ainf, &key, compar);
	check(!found);

	key = 0;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_find(ainf, &key, compar);
	check(!found);
	
	return true;
}

static bool test_arr_tint_bsearch(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};

	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 1, 1);

	int key = 2;
	int * found = arr_tint_bsearch(ainf, &key, compar);
	check(!found);

	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(!found);

	arr_tint_info_set(ainf, arr, 1, 2);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(1 == arr_tint_distance(ainf, found));
	check(found == (arr+1));

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(1 == arr_tint_distance(ainf, found));

	key = 4;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(3 == arr_tint_distance(ainf, found));

	key = 5;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(4 == arr_tint_distance(ainf, found));

	key = 1;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(0 == arr_tint_distance(ainf, found));

	key = -5;
	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(!found);

	key = 0;
	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(!found);

	key = 10;
	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_bsearch(ainf, &key, compar);
	check(!found);
	
	return true;
}

static bool test_arr_tint_sfirst_eq(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};

	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 1, 1);

	int key = 2;
	int * found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(!found);

	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(!found);

	arr_tint_info_set(ainf, arr, 1, 2);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(1 == arr_tint_distance(ainf, found));
	check(found == (arr+1));

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(1 == arr_tint_distance(ainf, found));

	key = 4;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(3 == arr_tint_distance(ainf, found));

	key = 5;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(4 == arr_tint_distance(ainf, found));

	key = 1;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(found);
	check(*(found) = key);
	check(0 == arr_tint_distance(ainf, found));

	key = -5;
	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(!found);

	key = 0;
	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(!found);

	key = 10;
	arr_tint_info_set(ainf, arr, 0, 1);
	found = arr_tint_sfirst_eq(ainf, &key, compar);
	check(!found);
	
	return true;
}

static bool test_arr_tint_sfirst_geq(void)
{
#undef ASIZE
#define ASIZE 7

	{
		int arr[2] = {1, 2};

		arr_tint_info ainf_, * ainf = &ainf_;
		arr_tint_info_set(ainf, arr, 0, 2);

		int key = 2;
		int * found = arr_tint_sfirst_geq(ainf, &key, compar);
		check(found);
		check(*(found) == key);
		check(1 == arr_tint_distance(ainf, found));

		key = 1;
		found = arr_tint_sfirst_geq(ainf, &key, compar);
		check(found);
		check(*(found) == key);
		check(0 == arr_tint_distance(ainf, found));

		key = 0;
		found = arr_tint_sfirst_geq(ainf, &key, compar);
		check(found);
		check(*(found) == 1);
		check(0 == arr_tint_distance(ainf, found));

		key = 3;
		found = arr_tint_sfirst_geq(ainf, &key, compar);
		check(!found);
	}
	
	int arr[ASIZE] = {1, 2, 2, 3, 4, 4, 5};
	
	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 0, ASIZE);

	int key = 2;
	int * found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == key);
	check(1 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 1, ASIZE);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == key);
	check(1 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 2, ASIZE);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == key);
	check(2 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 4, ASIZE);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == 4);
	check(4 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 1, 2);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == key);
	check(1 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 1, 1);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(!found);

	key = 4;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == 4);
	check(4 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 0, ASIZE-1);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == 4);
	check(4 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 0, ASIZE-2);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == 4);
	check(4 == arr_tint_distance(ainf, found));

	key = 5;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == key);
	check(6 == arr_tint_distance(ainf, found));

	key = 5;
	arr_tint_info_set(ainf, arr, 0, ASIZE-1);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(!found);

	key = 1000;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(!found);

	key = -10;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_geq(ainf, &key, compar);
	check(found);
	check(*(found) == 1);
	check(0 == arr_tint_distance(ainf, found));
	
	return true;
	
#undef ASIZE
#define ASIZE 5
}

static bool test_arr_tint_sfirst_gt(void)
{
	#undef ASIZE
#define ASIZE 7

	{
		int arr[2] = {1, 2};

		arr_tint_info ainf_, * ainf = &ainf_;
		arr_tint_info_set(ainf, arr, 0, 2);

		int key = 2;
		int * found = arr_tint_sfirst_gt(ainf, &key, compar);
		check(!found);

		key = 1;
		found = arr_tint_sfirst_gt(ainf, &key, compar);
		check(found);
		check(*(found) == 2);
		check(1 == arr_tint_distance(ainf, found));

		key = 0;
		found = arr_tint_sfirst_gt(ainf, &key, compar);
		check(found);
		check(*(found) == 1);
		check(0 == arr_tint_distance(ainf, found));

		key = 3;
		found = arr_tint_sfirst_gt(ainf, &key, compar);
		check(!found);
	}
	
	int arr[ASIZE] = {1, 2, 2, 3, 4, 4, 5};
	
	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 0, ASIZE);

	int key = 2;
	int * found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*(found) == 3);
	check(3 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 1, ASIZE);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*(found) == 3);
	check(3 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 2, ASIZE);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*(found) == 3);
	check(3 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 4, ASIZE);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*(found) == 4);
	check(4 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 1, 2);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(!found);

	arr_tint_info_set(ainf, arr, 1, 1);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(!found);

	key = 4;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*(found) == 5);
	check(6 == arr_tint_distance(ainf, found));

	arr_tint_info_set(ainf, arr, 0, ASIZE-1);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(!found);
	
	arr_tint_info_set(ainf, arr, 0, ASIZE-2);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(!found);
	
	key = 5;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(!found);
	
	arr_tint_info_set(ainf, arr, 0, ASIZE-1);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(!found);

	key = 1000;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(!found);

	key = -10;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	found = arr_tint_sfirst_gt(ainf, &key, compar);
	check(found);
	check(*(found) == 1);
	check(0 == arr_tint_distance(ainf, found));
	
	return true;
	
#undef ASIZE
#define ASIZE 5
}

static bool test_arr_tint_swap(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	int res1[ASIZE] = {1, 2, 3, 4, 5};
	int res2[ASIZE] = {5, 2, 3, 4, 1};
	int res3[ASIZE] = {1, 4, 3, 2, 5};
	
	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	
	arr_tint_swap(ainf, 0, 0);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	arr_tint_swap(ainf, 2, 2);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_swap(ainf, 0, 4);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	arr_tint_swap(ainf, 0, 4);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	// the range should be ignored
	arr_tint_info_set(ainf, arr, 4, ASIZE);
	arr_tint_swap(ainf, 0, 4);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	arr_tint_swap(ainf, 0, 4);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	arr_tint_swap(ainf, 4, 0);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);
	arr_tint_swap(ainf, 4, 0);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_swap(ainf, 1, 3);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);
	arr_tint_swap(ainf, 3, 1);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool test_arr_tint_reverse(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	int res1[ASIZE] = {1, 2, 3, 4, 5};
	int res2[ASIZE] = {1, 2, 4, 3, 5};
	int res3[ASIZE] = {5, 4, 3, 2, 1};
	int res4[ASIZE] = {1, 2, 5, 4, 3};
	int res5[ASIZE] = {2, 1, 3, 4, 5};
	
	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 2, 2);

	arr_tint_reverse(ainf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 2, 3);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 2, 4);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 2, 4);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res3, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 2, ASIZE);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res4, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 2, ASIZE);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, 1);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, 2);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res5, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, 2);
	arr_tint_reverse(ainf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	
	return true;
}

static bool test_arr_tint_apply(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	int res1[ASIZE] = {1, 2, 3, 4, 5};
	int res2[ASIZE] = {2, 3, 4, 5, 6};
	
	arr_tint_info ainf_, * ainf = &ainf_;
	arr_tint_info_set(ainf, arr, 0, 0);

	int out = 0;
	check(0 == out);
	arr_tint_apply(ainf, accumulate, &out);
	check(0 == out);

	out = 0;
	check(0 == out);
	arr_tint_info_set(ainf, arr, 0, 1);
	arr_tint_apply(ainf, accumulate, &out);
	check(1 == out);

	out = 0;
	check(0 == out);
	arr_tint_info_set(ainf, arr, 0, 2);
	arr_tint_apply(ainf, accumulate, &out);
	check(3 == out);

	out = 0;
	check(0 == out);
	arr_tint_info_set(ainf, arr, 0, 2);
	arr_tint_apply(ainf, accumulate, &out);
	check(3 == out);

	out = 0;
	check(0 == out);
	arr_tint_info_set(ainf, arr, 4, ASIZE);
	arr_tint_apply(ainf, accumulate, &out);
	check(5 == out);

	out = 0;
	check(0 == out);
	arr_tint_info_set(ainf, arr, 3, ASIZE);
	arr_tint_apply(ainf, accumulate, &out);
	check(9 == out);

	out = 0;
	check(0 == out);
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	arr_tint_apply(ainf, accumulate, &out);
	check(15 == out);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	arr_tint_apply(ainf, incr, NULL);
	check(memcmp(arr, res2, ASIZE*sizeof(int)) == 0);

	arr_tint_info_set(ainf, arr, 0, ASIZE);
	arr_tint_apply(ainf, decr, NULL);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);

	foo pf_ = {3, 0, false}, * pf = &pf_;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	arr_tint_apply(ainf, myfind, pf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	check(3 == pf->a && 2 == pf->b && pf->found);

	memset(pf, 0, sizeof(*pf));
	check(0 == pf->a && 0 == pf->b && !pf->found);
	pf->a = 2;
	arr_tint_info_set(ainf, arr, 0, ASIZE);
	arr_tint_apply(ainf, myfind, pf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	check(2 == pf->a && 1 == pf->b && pf->found);

	memset(pf, 0, sizeof(*pf));
	check(0 == pf->a && 0 == pf->b && !pf->found);
	pf->a = 2;
	arr_tint_info_set(ainf, arr, 2, ASIZE);
	arr_tint_apply(ainf, myfind, pf);
	check(memcmp(arr, res1, ASIZE*sizeof(int)) == 0);
	check(2 == pf->a && 3 == pf->b && !pf->found);
	
	return true;
}

static bool test_arr_tint_get(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	check(1 == *arr_tint_get(arr, 0));
	check(2 == *arr_tint_get(arr, 1));
	check(3 == *arr_tint_get(arr, 2));
	check(4 == *arr_tint_get(arr, 3));
	check(5 == *arr_tint_get(arr, 4));
	return true;
}

static bool test_arr_tint_write(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	
	int elem = 10;
	check(10 == *arr_tint_write(arr, 0, &elem));

	elem = 20;
	check(20 == *arr_tint_write(arr, 1, &elem));
	
	elem = 30;
	check(30 == *arr_tint_write(arr, 2, &elem));
	
	elem = 40;
	check(40 == *arr_tint_write(arr, 3, &elem));
	
	elem = 50;
	check(50 == *arr_tint_write(arr, 4, &elem));
	return true;
}

static bool test_arr_tint_is_eq(void)
{
	int arr[ASIZE] = {1, 2, 3, 4, 5};
	int arr_same[ASIZE] = {1, 2, 3, 4, 5};
	int arr_diff[ASIZE] = {0, 2, 3, 4, 0};
	
	arr_tint_info arr_inf_, * arr_inf = &arr_inf_;
	arr_tint_info_set(arr_inf, arr, 0, ASIZE);
	
	arr_tint_info arr_same_inf_, * arr_same_inf = &arr_same_inf_;
	arr_tint_info_set(arr_same_inf, arr_same, 0, ASIZE);
	
	check(arr_tint_is_eq(arr_inf, arr_same_inf, compar));
	
	arr_tint_info arr_diff_inf_, * arr_diff_inf = &arr_diff_inf_;
	arr_tint_info_set(arr_diff_inf, arr_diff, 0, ASIZE);
	
	check(!arr_tint_is_eq(arr_inf, arr_diff_inf, compar));
	
	arr_tint_info_set(arr_diff_inf, arr_diff, 1, 4);
	arr_tint_info_set(arr_inf, arr, 1, 4);
	check(arr_tint_is_eq(arr_inf, arr_diff_inf, compar));
	
	arr_tint_info_set(arr_diff_inf, arr_diff, 0, 0);
	arr_tint_info_set(arr_inf, arr, 0, 0);
	check(arr_tint_is_eq(arr_inf, arr_diff_inf, compar));
	
	return true;
}

static ftest tests[] = {
	test_arr_void_info_init,
	test_arr_void_distance,
	test_arr_void_set_to,
	test_arr_void_copy,
	test_arr_void_move,
	test_arr_void_is_sorted,
	test_arr_void_sort,
	test_arr_void_partition,
	test_arr_void_find,
	test_arr_void_bsearch,
	test_arr_void_sfirst_eq,
	test_arr_void_sfirst_geq,
	test_arr_void_sfirst_gt,
	test_arr_void_swap,
	test_arr_void_reverse,
	test_arr_void_apply,
	test_arr_void_get,
	test_arr_void_write,
	test_arr_void_is_eq,
	test_arr_tint_info_init,
	test_arr_tint_result_diff,
	test_arr_tint_set_to,
	test_arr_tint_copy,
	test_arr_tint_move,
	test_arr_tint_is_sorted,
	test_arr_tint_sort,
	test_arr_tint_partition,
	test_arr_tint_find,
	test_arr_tint_bsearch,
	test_arr_tint_sfirst_eq,
	test_arr_tint_sfirst_geq,
	test_arr_tint_sfirst_gt,
	test_arr_tint_swap,
	test_arr_tint_reverse,
	test_arr_tint_apply,
	test_arr_tint_get,
	test_arr_tint_write,
	test_arr_tint_is_eq,
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
