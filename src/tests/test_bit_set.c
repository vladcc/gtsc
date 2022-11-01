#include <stdio.h>
#include <string.h>
#include "test.h"
#include "bit_set/bit_set.h"

static bool test_bytes_bits(void)
{
	check(0 == bset_byte_of(0));
	check(0 == bset_byte_of(0));
	check(0 == bset_tail_bits(0));
	
	check(1 == bset_bytes(1));
	check(0 == bset_byte_of(1));
	check(1 == bset_tail_bits(1));
	
	check(1 == bset_bytes(3));
	check(0 == bset_byte_of(3));
	check(3 == bset_tail_bits(3));
	
	check(1 == bset_bytes(7));
	check(0 == bset_byte_of(7));
	check(7 == bset_tail_bits(7));
	
	check(1 == bset_bytes(8));
	check(1 == bset_byte_of(8));
	check(0 == bset_tail_bits(8));
	
	check(2 == bset_bytes(9));
	check(1 == bset_byte_of(9));
	check(1 == bset_tail_bits(9));
	
	check(2 == bset_bytes(13));
	check(1 == bset_byte_of(13));
	check(5 == bset_tail_bits(13));

	check(2 == bset_bytes(16));
	check(2 == bset_byte_of(16));
	check(0 == bset_tail_bits(16));
	
	check(3 == bset_bytes(17));
	check(2 == bset_byte_of(17));
	check(1 == bset_tail_bits(17));
	
	return true;
}

static bool test_bit_set_create_destroy_num_bits(void)
{
	bit_set set_, * set = &set_;

	bit_set zero;
	memset(&zero, 0, sizeof(zero));
	
	uint8_t arr_1[1] = {0};
	check(set == bset_create(set, 1));
	check(set->num_bits == 1);
	check(bset_num_bits(set) == 1);
	check(memcmp(set->bits, arr_1, 1) == 0);

	bset_destroy(set);
	check(memcmp(set, &zero, sizeof(bit_set)) == 0);
	
	check(set == bset_create(set, 8));
	check(set->num_bits == 8);
	check(bset_num_bits(set) == 8);
	bset_destroy(set);

	check(set == bset_create(set, 9));
	check(set->num_bits == 9);
	check(bset_num_bits(set) == 9);
	bset_destroy(set);
	
	return true;
}

static bool test_is_copy_eq(void)
{
	bit_set seta_, * seta = &seta_;
	check(seta == bset_create(seta, 8));
	
	bit_set setb_, * setb = &setb_;
	check(setb == bset_create(setb, 8));
	
	check(bset_is_eq(seta, seta));
	check(bset_is_eq(setb, setb));
	check(bset_is_eq(seta, setb));
	check(bset_is_eq(setb, seta));
	
	{
		bit_set cpy_, * cpy = &cpy_;
		bool is_ok = false;
		
		*cpy = bset_copy(seta, &is_ok);
		check(is_ok);
		check(bset_is_eq(cpy, seta));
		check(bset_is_eq(seta, cpy));
		
		bset_destroy(cpy);
	}

	bset_set(setb, 1);
	check(!bset_is_eq(setb, seta));
	bset_clear(setb, 1);
	check(bset_is_eq(seta, setb));
	
	bset_toggle(setb, 1);
	check(!bset_is_eq(seta, setb));
	bset_toggle(setb, 1);
	check(bset_is_eq(setb, seta));
	
	bset_destroy(setb);
	check(setb == bset_create(setb, 9));
	check(!bset_is_eq(seta, setb));
	check(!bset_is_eq(setb, seta));
	
	bset_destroy(seta);
	check(seta == bset_create(seta, 9));
	check(bset_is_eq(seta, setb));
	
	bset_all_set(seta);
	check(!bset_is_eq(seta, setb));
	bset_all_clear(seta);
	check(bset_is_eq(seta, setb));
	
	{
		bit_set cpy_, * cpy = &cpy_;
		bool is_ok = false;
		
		*cpy = bset_copy(seta, &is_ok);
		check(is_ok);
		check(bset_is_eq(cpy, seta));
		check(bset_is_eq(seta, cpy));
		
		bset_destroy(cpy);
	}
	
	bset_destroy(setb);
	check(setb == bset_create(setb, 13));
	check(!bset_is_eq(seta, setb));
	check(!bset_is_eq(setb, seta));
	
	bset_destroy(seta);
	check(seta == bset_create(seta, 13));
	check(bset_is_eq(seta, setb));
	
	bset_all_set(seta);
	check(!bset_is_eq(seta, setb));
	bset_all_clear(seta);
	check(bset_is_eq(seta, setb));
	
	bset_destroy(seta);
	bset_destroy(setb);
	
	return true;
}

static bool test_check_set_clear_toggle(void)
{	
	bit_set set_, * set = &set_;
	
	{
		check(set == bset_create(set, 8));
		
		for (size_t i = 0; i < 8; ++i)
			check(!bset_check(set, i));
		
		check(!bset_check(set, 0));
		check(!bset_check(set, 1));
		check(!bset_check(set, 2));
		check(!bset_check(set, 3));
		check(!bset_check(set, 4));
		check(!bset_check(set, 5));
		check(!bset_check(set, 6));
		check(!bset_check(set, 7));
		
		bset_set(set, 0);
		bset_set(set, 3);
		bset_set(set, 7);
		
		check(bset_check(set, 0));
		check(!bset_check(set, 1));
		check(!bset_check(set, 2));
		check(bset_check(set, 3));
		check(!bset_check(set, 4));
		check(!bset_check(set, 5));
		check(!bset_check(set, 6));
		check(bset_check(set, 7));
		
		bset_set(set, 0);
		bset_set(set, 3);
		bset_set(set, 7);
		
		check(bset_check(set, 0));
		check(!bset_check(set, 1));
		check(!bset_check(set, 2));
		check(bset_check(set, 3));
		check(!bset_check(set, 4));
		check(!bset_check(set, 5));
		check(!bset_check(set, 6));
		check(bset_check(set, 7));
		
		bset_clear(set, 0);
		bset_clear(set, 3);
		bset_clear(set, 7);
		
		check(!bset_check(set, 0));
		check(!bset_check(set, 1));
		check(!bset_check(set, 2));
		check(!bset_check(set, 3));
		check(!bset_check(set, 4));
		check(!bset_check(set, 5));
		check(!bset_check(set, 6));
		check(!bset_check(set, 7));
		
		bset_clear(set, 0);
		bset_clear(set, 3);
		bset_clear(set, 7);
		
		check(!bset_check(set, 0));
		check(!bset_check(set, 1));
		check(!bset_check(set, 2));
		check(!bset_check(set, 3));
		check(!bset_check(set, 4));
		check(!bset_check(set, 5));
		check(!bset_check(set, 6));
		check(!bset_check(set, 7));

		bset_toggle(set, 0);
		bset_toggle(set, 3);
		bset_toggle(set, 7);
		
		check(bset_check(set, 0));
		check(!bset_check(set, 1));
		check(!bset_check(set, 2));
		check(bset_check(set, 3));
		check(!bset_check(set, 4));
		check(!bset_check(set, 5));
		check(!bset_check(set, 6));
		check(bset_check(set, 7));
		
		bset_toggle(set, 0);
		bset_toggle(set, 3);
		bset_toggle(set, 7);
		
		check(!bset_check(set, 0));
		check(!bset_check(set, 1));
		check(!bset_check(set, 2));
		check(!bset_check(set, 3));
		check(!bset_check(set, 4));
		check(!bset_check(set, 5));
		check(!bset_check(set, 6));
		check(!bset_check(set, 7));
		
		bset_destroy(set);
	}
	
	{
		const size_t bnum = 20;
		check(set == bset_create(set, bnum));
		
		for (size_t i = 0; i < bnum; ++i)
			check(!bset_check(set, i));
		
		for (size_t i = 0; i < bnum; ++i)
			bset_set(set, i);	
		
		for (size_t i = 0; i < bnum; ++i)
			check(bset_check(set, i));
		
		for (size_t i = 0; i < bnum; ++i)
			bset_clear(set, i);
		
		for (size_t i = 0; i < bnum; ++i)
			check(!bset_check(set, i));
		
		for (size_t i = 0; i < bnum; ++i)
			bset_toggle(set, i);
		
		for (size_t i = 0; i < bnum; ++i)
			check(bset_check(set, i));
		
		for (size_t i = 0; i < bnum; ++i)
			bset_toggle(set, i);
		
		for (size_t i = 0; i < bnum; ++i)
			check(!bset_check(set, i));
		
		bset_destroy(set);
	}
	
	return true;
}

static bool test_bit_set_set_to_pattern_zeroes_ones(void)
{
	bit_set set_, * set = &set_;

	{
		typedef struct byte_1 {
			uint8_t num_bits;
			uint8_t bit_pat;
			uint8_t byte_val;
			uint8_t exp_zeroes;
			uint8_t exp_ones;
		} byte_1;
		
		// when the bit set has only one byte, it is always the last byte
		
		byte_1 tbl_1[] = {
			{8, 0xFF, 0xFF, 0, 8}, // 1111_1111
			{7, 0xFF, 0x7F, 0, 7}, // 0 111_1111
			{6, 0xFF, 0x3F, 0, 6}, // 00 11_1111
			{5, 0xFF, 0x1F, 0, 5}, // 000 1_1111
			{4, 0xFF, 0x0F, 0, 4}, // 0000_ 1111
			{3, 0xFF, 0x07, 0, 3}, // 0000_0 111
			{2, 0xFF, 0x03, 0, 2}, // 0000_00 11
			{1, 0xFF, 0x01, 0, 1}, // 0000_000 1
			{8, 0xAA, 0xAA, 4, 4}, // 1010_1010
			{7, 0xAA, 0x2A, 4, 3}, // 0 010_1010
			{6, 0xAA, 0x2A, 3, 3}, // 00 10_1010
			{5, 0xAA, 0x0A, 3, 2}, // 000 0_1010
			{4, 0xAA, 0x0A, 2, 2}, // 0000_ 1010
			{3, 0xAA, 0x02, 2, 1}, // 0000_0 010
			{2, 0xAA, 0x02, 1, 1}, // 0000_00 10
			{1, 0xAA, 0x00, 1, 0}, // 0000_000 0
			{8, 0x55, 0x55, 4, 4}, // 0101_0101
			{7, 0x55, 0x55, 3, 4}, // 0 101_0101
			{6, 0x55, 0x15, 3, 3}, // 00 01_0101
			{5, 0x55, 0x15, 2, 3}, // 000 1_0101
			{4, 0x55, 0x05, 2, 2}, // 0000_ 0101
			{3, 0x55, 0x05, 1, 2}, // 0000_0 101
			{2, 0x55, 0x01, 1, 1}, // 0000_00 01
			{1, 0x55, 0x01, 0, 1}, // 0000_000 1
		};
		
		for (size_t i = 0; i < sizeof(tbl_1)/sizeof(*tbl_1); ++i)
		{
			byte_1 * pb1 = tbl_1 + i;
			
			check(set == bset_create(set, pb1->num_bits));
			check(pb1->num_bits == bset_num_bits(set));
			
			bset_set_to_pattern(set, pb1->bit_pat);
			check(pb1->byte_val == set->bits[0]);
			check(bset_num_zeroes(set) == pb1->exp_zeroes);
			check(bset_num_ones(set) == pb1->exp_ones);
			
			{
				bit_set cpy_, * cpy = &cpy_;
				bool is_ok = false;
				
				*cpy = bset_copy(set, &is_ok);
				check(is_ok);
				check(bset_is_eq(cpy, set));
				check(bset_is_eq(set, cpy));
				
				bset_destroy(cpy);
			}
			
			bset_destroy(set);
		}
	}
	
	{
		typedef struct byte_2 {
			uint8_t num_bits;
			uint8_t bit_pat;
			uint8_t byte_val[2];
			uint8_t exp_zeroes;
			uint8_t exp_ones;
		} byte_2;
		
		// check the last byte gets masked while the first one is the pattern
		
		byte_2 tbl_2[] = {
			{16, 0xFF, {0xFF, 0xFF}, 0, 16}, // 1111_1111 1111_1111
			{15, 0xFF, {0xFF, 0x7F}, 0, 15},
			{14, 0xFF, {0xFF, 0x3F}, 0, 14},
			{13, 0xFF, {0xFF, 0x1F}, 0, 13},
			{12, 0xFF, {0xFF, 0x0F}, 0, 12},
			{11, 0xFF, {0xFF, 0x07}, 0, 11},
			{10, 0xFF, {0xFF, 0x03}, 0, 10},
			{9, 0xFF, {0xFF, 0x01}, 0, 9},
			{16, 0xAA, {0xAA, 0xAA}, 8, 8}, // 1010_1010 1010_1010
			{15, 0xAA, {0xAA, 0x2A}, 8, 7},
			{14, 0xAA, {0xAA, 0x2A}, 7, 7},
			{13, 0xAA, {0xAA, 0x0A}, 7, 6},
			{12, 0xAA, {0xAA, 0x0A}, 6, 6},
			{11, 0xAA, {0xAA, 0x02}, 6, 5},
			{10, 0xAA, {0xAA, 0x02}, 5, 5},
			{9, 0xAA, {0xAA, 0x00}, 5, 4},
			{16, 0x55, {0x55, 0x55}, 8, 8}, // 0101_0101 0101_0101
			{15, 0x55, {0x55, 0x55}, 7, 8},
			{14, 0x55, {0x55, 0x15}, 7, 7},
			{13, 0x55, {0x55, 0x15}, 6, 7},
			{12, 0x55, {0x55, 0x05}, 6, 6},
			{11, 0x55, {0x55, 0x05}, 5, 6},
			{10, 0x55, {0x55, 0x01}, 5, 5},
			{9, 0x55, {0x55, 0x01}, 4, 5},
		};
		
		
		for (size_t i = 0; i < sizeof(tbl_2)/sizeof(*tbl_2); ++i)
		{
			byte_2 * pb2 = tbl_2 + i;
			
			check(set == bset_create(set, pb2->num_bits));
			check(pb2->num_bits == bset_num_bits(set));
			
			bset_set_to_pattern(set, pb2->bit_pat);
			check(pb2->byte_val[0] == set->bits[0]);
			check(pb2->byte_val[1] == set->bits[1]);
			check(bset_num_zeroes(set) == pb2->exp_zeroes);
			check(bset_num_ones(set) == pb2->exp_ones);
			
			{
				bit_set cpy_, * cpy = &cpy_;
				bool is_ok = false;
				
				*cpy = bset_copy(set, &is_ok);
				check(is_ok);
				check(bset_is_eq(cpy, set));
				check(bset_is_eq(set, cpy));
				
				bset_destroy(cpy);
			}
			
			bset_destroy(set);
		}
	}
	
	return true;
}

static bool test_bit_set_all_set_all_zero(void)
{
	bit_set set_, * set = &set_;
	
	{
		check(set == bset_create(set, 8));
		check(0 == set->bits[0]);
		
		bset_all_set(set);
		check(0xFF == set->bits[0]);
		bset_all_clear(set);
		check(0 == set->bits[0]);
		
		bset_destroy(set);
	}	
	
	{
		check(set == bset_create(set, 5));
		check(0 == set->bits[0]);
		
		bset_all_set(set);
		check(0x1F == set->bits[0]);
		bset_all_clear(set);
		check(0 == set->bits[0]);
		
		bset_destroy(set);
	}
	
	{
		check(set == bset_create(set, 13));
		check(0 == set->bits[0]);
		check(0 == set->bits[1]);
		
		bset_all_set(set);
		check(0xFF == set->bits[0]);
		check(0x1F == set->bits[1]);
		bset_all_clear(set);
		check(0 == set->bits[0]);
		check(0 == set->bits[1]);
		
		bset_destroy(set);
	}
	
	{
		check(set == bset_create(set, 16));
		check(0 == set->bits[0]);
		check(0 == set->bits[1]);
		
		bset_all_set(set);
		check(0xFF == set->bits[0]);
		check(0xFF == set->bits[1]);
		bset_all_clear(set);
		check(0 == set->bits[0]);
		check(0 == set->bits[1]);
		
		bset_destroy(set);
	}
	
	return true;
}

static ftest tests[] = {
	test_bytes_bits,
	test_bit_set_create_destroy_num_bits,
	test_is_copy_eq,
	test_check_set_clear_toggle,
	test_bit_set_set_to_pattern_zeroes_ones,
	test_bit_set_all_set_all_zero,
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
