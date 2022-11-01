#include <stdio.h>
#include "test.h"
#include "vset/vset.h"

static int compar(const void * ka, const void * kb)
{
	int a = *((int *)ka);
	int b = *((int *)kb);
	return (a > b) - (a < b);
}

#define ASIZE 10

static bool test_vset_void_create(void)
{
	vset_void set_, * set = &set_;
	int arr[ASIZE] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
	int arr_2[ASIZE/2] = {1, 2, 3, 4, 5};

	check(set == vset_void_create_cap(set, sizeof(int), compar, 10));
	check(vset_void_size(set) == 0);
	check(vset_void_cap(set) == 10);
	check(vset_void_is_empty(set));
	check(vset_void_compar(set) == compar);
	check(vset_void_data(set) == set->vect.data);
	check(vset_void_vect(set) == &set->vect);

	vset_void_destroy(set);
	vset_void zero;
	memset(&zero, 0, sizeof(zero));
	check(memcmp(set, &zero, sizeof(vset_void)) == 0);
	
	check(set == vset_void_create(set, sizeof(int), compar));
	check(vset_void_size(set) == 0);
	check(vset_void_cap(set) == 32);
	check(vset_void_is_empty(set));
	check(vset_void_compar(set) == compar);
	check(vset_void_data(set) == set->vect.data);
	check(vset_void_vect(set) == &set->vect);
	vset_void_destroy(set);

	check(set == vset_void_create_arr(set, sizeof(int), compar, arr, ASIZE));
	check(vset_void_size(set) == 5);
	check(!vset_void_is_empty(set));
	check(memcmp(vset_void_data(set), arr_2, (ASIZE/2)*sizeof(int)) == 0);
	vset_void_destroy(set);
	
	return true;
}

static bool test_vset_void_insert_erase_has_reset(void)
{
	vset_void set_, * set = &set_;

	check(set == vset_void_create_cap(set, sizeof(int), compar, 1));
	check(vset_void_cap(set) == 1);
	check(vset_void_size(set) == 0);
	check(vset_void_is_empty(set));

	int key = 1;
	check(!vset_void_has(set, &key));
	
	vset_void_insert(set, &key);
	check(vset_void_cap(set) == 1);
	check(vset_void_size(set) == 1);
	check(!vset_void_is_empty(set));

	check(1 == *((int *)vset_void_has(set, &key)));

	vset_void_insert(set, &key);
	check(vset_void_cap(set) == 1);
	check(vset_void_size(set) == 1);
	check(!vset_void_is_empty(set));

	vset_void_erase(set, &key);
	check(vset_void_cap(set) == 1);
	check(vset_void_size(set) == 0);
	check(vset_void_is_empty(set));

	vset_void_insert(set, &key);
	check(1 == *((int *)vset_void_has(set, &key)));
	vset_void_insert(set, &key);
	key = 2;
	vset_void_insert(set, &key);
	check(2 == *((int *)vset_void_has(set, &key)));
	vset_void_insert(set, &key);
	key = 3;
	vset_void_insert(set, &key);
	check(3 == *((int *)vset_void_has(set, &key)));
	vset_void_insert(set, &key);

	check(vset_void_cap(set) == 4);
	check(vset_void_size(set) == 3);
	check(!vset_void_is_empty(set));

	key = 2;
	vset_void_erase(set, &key);
	check(vset_void_cap(set) == 4);
	check(vset_void_size(set) == 2);
	check(!vset_void_is_empty(set));
	check(!vset_void_has(set, &key));
	
	vset_void_reset(set);
	key = 1;
	check(!vset_void_has(set, &key));
	key = 2;
	check(!vset_void_has(set, &key));
	key = 3;
	check(!vset_void_has(set, &key));

	check(vset_void_cap(set) == 4);
	check(vset_void_size(set) == 0);
	check(vset_void_is_empty(set));

	vset_void_destroy(set);
	
	return true;
}

static bool test_vset_void_get_copy_is_eq_insert_arr_stf(void)
{
	vset_void set_, * set = &set_;
	int arr[ASIZE] = {6, 4, 3, 2, 1, 1, 2, 3, 4, 6};
	int arr_2[ASIZE/2] = {1, 2, 3, 4, 6};
	
	check(set == vset_void_create(set, sizeof(int), compar));
	check(vset_void_size(set) == 0);
	check(vset_void_cap(set) == 32);
	check(vset_void_is_empty(set));
	check(vset_void_compar(set) == compar);
	check(vset_void_data(set) == set->vect.data);
	check(vset_void_vect(set) == &set->vect);

	check(set == vset_void_insert_arr(set, arr, ASIZE));
	check(vset_void_size(set) == 5);
	check(memcmp(vset_void_data(set), arr_2, (ASIZE/2)*sizeof(int)) == 0);

	check(vset_void_is_eq(set, set));

	bool is_ok = false;
	vset_void cpy = vset_void_copy(set, &is_ok);
	check(vset_void_size(&cpy) == 5);
	check(vset_void_cap(&cpy) == 5);
	check(memcmp(vset_void_data(&cpy), arr_2, (ASIZE/2)*sizeof(int)) == 0);
	check(is_ok);
	check(vset_void_is_eq(set, &cpy));

	check(1 == *((int *)vset_void_get(set, 0)));
	check(2 == *((int *)vset_void_get(set, 1)));
	check(3 == *((int *)vset_void_get(set, 2)));
	check(4 == *((int *)vset_void_get(set, 3)));
	check(6 == *((int *)vset_void_get(set, 4)));
	
	int key = 5;
	vset_void_insert(set, &key);
	int arr_3[ASIZE/2+1] = {1, 2, 3, 4, 5, 6};
	check(memcmp(vset_void_data(set), arr_3, (ASIZE/2+1)*sizeof(int)) == 0);
	check(vset_void_size(set) == 6);
	check(vset_void_cap(set) == 32);
	check(!vset_void_is_eq(set, &cpy));

	vset_void_shrink_to_fit(set);
	check(vset_void_size(set) == 6);
	check(vset_void_cap(set) == 6);

	vset_void_reset(set);
	check(vset_void_is_empty(set));
	check(vset_void_is_eq(set, set));
	check(!vset_void_is_eq(set, &cpy));
	
	vset_void_destroy(set);
	vset_void_destroy(&cpy);
	
	return true;
}

static bool test_vset_void_diff_un_isc(void)
{
	int arr_1[ASIZE] = {3, 4, 5, 1 ,2, 3, 5, 4, 2, 1};
	int arr_1_1[ASIZE/2] = {1, 2, 3, 4, 5};
	int arr_2[ASIZE] = {5, 4, 6, 8, 7, 7, 8, 5, 6, 4};
	int arr_2_1[ASIZE/2] = {4, 5, 6, 7, 8};

	vset_void empty_, * empty = &empty_;
	vset_void seta_, * seta = &seta_;
	vset_void setb_, * setb = &setb_;
	vset_void setc_, * setc = &setc_;

	check(empty == vset_void_create(empty, sizeof(int), compar));
	check(vset_void_is_empty(empty));

	check(seta ==
		vset_void_create_arr(seta, sizeof(int), compar, arr_1, ASIZE));
	check(memcmp(vset_void_data(seta), arr_1_1, (ASIZE/2)*sizeof(int)) == 0);

	check(setb ==
		vset_void_create_arr(setb, sizeof(int), compar, arr_2, ASIZE));
	check(memcmp(vset_void_data(setb), arr_2_1, (ASIZE/2)*sizeof(int)) == 0);

	check(setc == vset_void_create(setc, sizeof(int), compar));
	check(vset_void_size(setc) == 0);
	check(vset_void_cap(setc) == 32);
	
	int arr_diff[] = {1, 2, 3};
	check(setc == vset_void_diff(setc, seta, setb));
	check(vset_void_size(setc) == 3);
	check(vset_void_cap(setc) == 32);
	check(memcmp(vset_void_data(setc), arr_diff, 3*sizeof(int)) == 0);

	check(setc == vset_void_diff(setc, empty, setb));
	check(vset_void_is_empty(setc));
	check(vset_void_is_eq(setc, empty));

	check(setc == vset_void_diff(setc, setb, empty));
	check(vset_void_is_eq(setc, setb));

	int arr_diff_2[] = {6, 7, 8};
	check(setc == vset_void_diff(setc, setb, seta));
	check(vset_void_size(setc) == 3);
	check(vset_void_cap(setc) == 32);
	check(memcmp(vset_void_data(setc), arr_diff_2, 3*sizeof(int)) == 0);

	int arr_isc[] = {4, 5};
	check(setc == vset_void_intersect(setc, seta, setb));
	check(vset_void_size(setc) == 2);
	check(vset_void_cap(setc) == 32);
	check(memcmp(vset_void_data(setc), arr_isc, 2*sizeof(int)) == 0);

	vset_void_reset(setc);
	check(setc == vset_void_intersect(setc, setb, seta));
	check(memcmp(vset_void_data(setc), arr_isc, 2*sizeof(int)) == 0);

	check(setc == vset_void_intersect(setc, empty, seta));
	check(vset_void_is_empty(setc));
	check(vset_void_is_eq(setc, empty));

	check(setc == vset_void_intersect(setc, setb, empty));
	check(vset_void_is_empty(setc));
	check(vset_void_is_eq(setc, empty));
	
	int arr_un[] = {1, 2, 3, 4, 5, 6, 7, 8};
	check(setc == vset_void_union(setc, seta, setb));
	check(vset_void_size(setc) == 8);
	check(vset_void_cap(setc) == 32);
	check(memcmp(vset_void_data(setc), arr_un, 8*sizeof(int)) == 0);

	vset_void_reset(setc);
	check(setc == vset_void_union(setc, setb, seta));
	check(memcmp(vset_void_data(setc), arr_un, 2*sizeof(int)) == 0);

	check(setc == vset_void_union(setc, setb, empty));
	check(vset_void_is_eq(setc, setb));

	check(setc == vset_void_union(setc, empty, seta));
	check(vset_void_is_eq(setc, seta));

	vset_void_destroy(empty);
	vset_void_destroy(seta);
	vset_void_destroy(setb);
	vset_void_destroy(setc);
	
	return true;
}

VSET_DEFINE(tint, int);

static bool test_vset_tint_create(void)
{
	vset_tint set_, * set = &set_;
	int arr[ASIZE] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
	int arr_2[ASIZE/2] = {1, 2, 3, 4, 5};

	check(set == vset_tint_create_cap(set, compar, 10));
	
	vset_void * base = vset_tint_get_base(set);
	check(&set->set == base);
	
	check(vset_tint_size(set) == 0);
	check(vset_tint_cap(set) == 10);
	check(vset_tint_is_empty(set));
	check(vset_tint_compar(set) == compar);
	check(vset_tint_data(set) == set->set.vect.data);
	check(vset_tint_vect(set) == &set->set.vect);

	vset_tint_destroy(set);
	vset_tint zero;
	memset(&zero, 0, sizeof(zero));
	check(memcmp(set, &zero, sizeof(vset_tint)) == 0);
	
	check(set == vset_tint_create(set, compar));
	check(vset_tint_size(set) == 0);
	check(vset_tint_cap(set) == 32);
	check(vset_tint_is_empty(set));
	check(vset_tint_compar(set) == compar);
	check(vset_tint_data(set) == set->set.vect.data);
	check(vset_tint_vect(set) == &set->set.vect);
	vset_tint_destroy(set);

	check(set == vset_tint_create_arr(set, compar, arr, ASIZE));
	check(vset_tint_size(set) == 5);
	check(!vset_tint_is_empty(set));
	check(memcmp(vset_tint_data(set), arr_2, (ASIZE/2)*sizeof(int)) == 0);
	vset_tint_destroy(set);
	
	return true;
}

static bool test_vset_tint_insert_erase_has_reset(void)
{
	vset_tint set_, * set = &set_;

	check(set == vset_tint_create_cap(set, compar, 1));
	check(vset_tint_cap(set) == 1);
	check(vset_tint_size(set) == 0);
	check(vset_tint_is_empty(set));

	int key = 1;
	check(!vset_tint_has(set, &key));
	
	vset_tint_insert(set, &key);
	check(vset_tint_cap(set) == 1);
	check(vset_tint_size(set) == 1);
	check(!vset_tint_is_empty(set));

	check(1 == *(vset_tint_has(set, &key)));

	vset_tint_insert(set, &key);
	check(vset_tint_cap(set) == 1);
	check(vset_tint_size(set) == 1);
	check(!vset_tint_is_empty(set));

	vset_tint_erase(set, &key);
	check(vset_tint_cap(set) == 1);
	check(vset_tint_size(set) == 0);
	check(vset_tint_is_empty(set));

	vset_tint_insert(set, &key);
	check(1 == *(vset_tint_has(set, &key)));
	vset_tint_insert(set, &key);
	key = 2;
	vset_tint_insert(set, &key);
	check(2 == *(vset_tint_has(set, &key)));
	vset_tint_insert(set, &key);
	key = 3;
	vset_tint_insert(set, &key);
	check(3 == *(vset_tint_has(set, &key)));
	vset_tint_insert(set, &key);

	check(vset_tint_cap(set) == 4);
	check(vset_tint_size(set) == 3);
	check(!vset_tint_is_empty(set));

	key = 2;
	vset_tint_erase(set, &key);
	check(vset_tint_cap(set) == 4);
	check(vset_tint_size(set) == 2);
	check(!vset_tint_is_empty(set));
	check(!vset_tint_has(set, &key));
	
	vset_tint_reset(set);
	key = 1;
	check(!vset_tint_has(set, &key));
	key = 2;
	check(!vset_tint_has(set, &key));
	key = 3;
	check(!vset_tint_has(set, &key));

	check(vset_tint_cap(set) == 4);
	check(vset_tint_size(set) == 0);
	check(vset_tint_is_empty(set));

	vset_tint_destroy(set);
	
	return true;
}

static bool test_vset_tint_get_copy_is_eq_insert_arr_stf(void)
{
	vset_tint set_, * set = &set_;
	int arr[ASIZE] = {6, 4, 3, 2, 1, 1, 2, 3, 4, 6};
	int arr_2[ASIZE/2] = {1, 2, 3, 4, 6};
	
	check(set == vset_tint_create(set, compar));
	check(vset_tint_size(set) == 0);
	check(vset_tint_cap(set) == 32);
	check(vset_tint_is_empty(set));
	check(vset_tint_compar(set) == compar);
	check(vset_tint_data(set) == set->set.vect.data);
	check(vset_tint_vect(set) == &set->set.vect);

	check(set == vset_tint_insert_arr(set, arr, ASIZE));
	check(vset_tint_size(set) == 5);
	check(memcmp(vset_tint_data(set), arr_2, (ASIZE/2)*sizeof(int)) == 0);

	check(vset_tint_is_eq(set, set));

	bool is_ok = false;
	vset_tint cpy = vset_tint_copy(set, &is_ok);
	check(vset_tint_size(&cpy) == 5);
	check(vset_tint_cap(&cpy) == 5);
	check(memcmp(vset_tint_data(&cpy), arr_2, (ASIZE/2)*sizeof(int)) == 0);
	check(is_ok);
	check(vset_tint_is_eq(set, &cpy));

	check(1 == *(vset_tint_get(set, 0)));
	check(2 == *(vset_tint_get(set, 1)));
	check(3 == *(vset_tint_get(set, 2)));
	check(4 == *(vset_tint_get(set, 3)));
	check(6 == *(vset_tint_get(set, 4)));
	
	int key = 5;
	vset_tint_insert(set, &key);
	int arr_3[ASIZE/2+1] = {1, 2, 3, 4, 5, 6};
	check(memcmp(vset_tint_data(set), arr_3, (ASIZE/2+1)*sizeof(int)) == 0);
	check(vset_tint_size(set) == 6);
	check(vset_tint_cap(set) == 32);
	check(!vset_tint_is_eq(set, &cpy));

	vset_tint_shrink_to_fit(set);
	check(vset_tint_size(set) == 6);
	check(vset_tint_cap(set) == 6);

	vset_tint_reset(set);
	check(vset_tint_is_empty(set));
	check(vset_tint_is_eq(set, set));
	check(!vset_tint_is_eq(set, &cpy));
	
	vset_tint_destroy(set);
	vset_tint_destroy(&cpy);
	
	return true;
}

static bool test_vset_tint_diff_un_isc(void)
{
	int arr_1[ASIZE] = {3, 4, 5, 1 ,2, 3, 5, 4, 2, 1};
	int arr_1_1[ASIZE/2] = {1, 2, 3, 4, 5};
	int arr_2[ASIZE] = {5, 4, 6, 8, 7, 7, 8, 5, 6, 4};
	int arr_2_1[ASIZE/2] = {4, 5, 6, 7, 8};

	vset_tint empty_, * empty = &empty_;
	vset_tint seta_, * seta = &seta_;
	vset_tint setb_, * setb = &setb_;
	vset_tint setc_, * setc = &setc_;

	check(empty == vset_tint_create(empty, compar));
	check(vset_tint_is_empty(empty));

	check(seta ==
		vset_tint_create_arr(seta, compar, arr_1, ASIZE));
	check(memcmp(vset_tint_data(seta), arr_1_1, (ASIZE/2)*sizeof(int)) == 0);

	check(setb ==
		vset_tint_create_arr(setb, compar, arr_2, ASIZE));
	check(memcmp(vset_tint_data(setb), arr_2_1, (ASIZE/2)*sizeof(int)) == 0);

	check(setc == vset_tint_create(setc, compar));
	check(vset_tint_size(setc) == 0);
	check(vset_tint_cap(setc) == 32);
	
	int arr_diff[] = {1, 2, 3};
	check(setc == vset_tint_diff(setc, seta, setb));
	check(vset_tint_size(setc) == 3);
	check(vset_tint_cap(setc) == 32);
	check(memcmp(vset_tint_data(setc), arr_diff, 3*sizeof(int)) == 0);

	check(setc == vset_tint_diff(setc, empty, setb));
	check(vset_tint_is_empty(setc));
	check(vset_tint_is_eq(setc, empty));

	check(setc == vset_tint_diff(setc, setb, empty));
	check(vset_tint_is_eq(setc, setb));

	int arr_diff_2[] = {6, 7, 8};
	check(setc == vset_tint_diff(setc, setb, seta));
	check(vset_tint_size(setc) == 3);
	check(vset_tint_cap(setc) == 32);
	check(memcmp(vset_tint_data(setc), arr_diff_2, 3*sizeof(int)) == 0);

	int arr_isc[] = {4, 5};
	check(setc == vset_tint_intersect(setc, seta, setb));
	check(vset_tint_size(setc) == 2);
	check(vset_tint_cap(setc) == 32);
	check(memcmp(vset_tint_data(setc), arr_isc, 2*sizeof(int)) == 0);

	vset_tint_reset(setc);
	check(setc == vset_tint_intersect(setc, setb, seta));
	check(memcmp(vset_tint_data(setc), arr_isc, 2*sizeof(int)) == 0);

	check(setc == vset_tint_intersect(setc, empty, seta));
	check(vset_tint_is_empty(setc));
	check(vset_tint_is_eq(setc, empty));

	check(setc == vset_tint_intersect(setc, setb, empty));
	check(vset_tint_is_empty(setc));
	check(vset_tint_is_eq(setc, empty));
	
	int arr_un[] = {1, 2, 3, 4, 5, 6, 7, 8};
	check(setc == vset_tint_union(setc, seta, setb));
	check(vset_tint_size(setc) == 8);
	check(vset_tint_cap(setc) == 32);
	check(memcmp(vset_tint_data(setc), arr_un, 8*sizeof(int)) == 0);

	vset_tint_reset(setc);
	check(setc == vset_tint_union(setc, setb, seta));
	check(memcmp(vset_tint_data(setc), arr_un, 2*sizeof(int)) == 0);

	check(setc == vset_tint_union(setc, setb, empty));
	check(vset_tint_is_eq(setc, setb));

	check(setc == vset_tint_union(setc, empty, seta));
	check(vset_tint_is_eq(setc, seta));

	vset_tint_destroy(empty);
	vset_tint_destroy(seta);
	vset_tint_destroy(setb);
	vset_tint_destroy(setc);
	
	return true;
}

static ftest tests[] = {
	test_vset_void_create,
	test_vset_void_insert_erase_has_reset,
	test_vset_void_get_copy_is_eq_insert_arr_stf,
	test_vset_void_diff_un_isc,
	test_vset_tint_create,
	test_vset_tint_insert_erase_has_reset,
	test_vset_tint_get_copy_is_eq_insert_arr_stf,
	test_vset_tint_diff_un_isc,
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
