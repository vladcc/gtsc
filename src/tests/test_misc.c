#include <stdio.h>
#include "test.h"

#include "misc/misc.h"

static bool test_next_p2(void)
{
	check(0 == misc_geq_p2(~0));
	check(1 == misc_geq_p2(0));
	check(1 == misc_geq_p2(1));
	check(2 == misc_geq_p2(2));
	check(4 == misc_geq_p2(3));
	check(16 == misc_geq_p2(10));
	check(16 == misc_geq_p2(16));
	check(32 == misc_geq_p2(17));
	
	return true;
}

static bool test_align_to(void)
{
	typedef struct algn {
		size_t n, align, res;
	} algn;

	algn arr[] = {
		{0, 3, 0},
		{1, 3, 3},
		{2, 3, 3},
		{3, 3, 3},
		{4, 3, 6},
		{5, 3, 6},
		{0, 8, 0},
		{1, 8, 8},
		{5, 8, 8},
		{8, 8, 8},
		{9, 8, 16},
		{16, 8, 16},
		{17, 8, 24},
		{24, 8, 24},
		{27, 8, 32},
	};

	bool is_ok = false;
	algn * p = NULL;
	for (size_t i = 0; i < sizeof(arr)/sizeof(*arr); ++i)
	{
		p = arr + i;
		is_ok = (p->res == misc_align_to(p->n, p->align));

		if (!is_ok)
			printf("out: %zu, n: %zu, align: %zu\n", p->res, p->n, p->align);

		check(is_ok);
	}
	
	return true;
}

static bool test_offset_in(void)
{
	check(5 == (size_t)misc_offset_in((void *)0, 5));
	check(7 == (size_t)misc_offset_in((void *)2, 5));
	check(2 == (size_t)misc_offset_in((void *)2, 0));
	return true;
}

static ftest tests[] = {
	test_next_p2,
	test_align_to,
	test_offset_in,
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
