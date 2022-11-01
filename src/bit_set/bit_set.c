#include "bit_set.h"
#include <stdlib.h>
#include <string.h>

bit_set * bset_create(bit_set * set, size_t num_bits)
{
	if ((set->bits = (uint8_t *)calloc(1, bset_bytes(num_bits))))
	{
		set->num_bits = num_bits;
		return set;
	}
	memset(set, 0, sizeof(*set));
	return NULL;
}

void bset_destroy(bit_set * set)
{
	free(set->bits);
	memset(set, 0, sizeof(*set));
}

bit_set bset_copy(const bit_set * set, bool * is_ok)
{
	*is_ok = false;
	bit_set cpy;
	size_t bits = set->num_bits;
	size_t bytes = bset_bytes(bits);
	if (bset_create(&cpy, bits))
	{
		memcpy(cpy.bits, set->bits, bytes);
		*is_ok = true;
	}
	return cpy;
}

bool bset_is_eq(const bit_set * a, const bit_set * b)
{
	if (a->num_bits == b->num_bits)
		return (memcmp(a->bits, b->bits, bset_bytes(a->num_bits)) == 0);
	return false;
}

void bset_set_to_pattern(bit_set * set, uint8_t pat)
{
	static const uint8_t last_byte_mask[8] = {
		0xFF, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F,
	};

	uint8_t * bits = set->bits;
	size_t nbits= set->num_bits;
	size_t bytes = bset_bytes(nbits);

	memset(bits, pat, bytes);
	bits[bytes-1] &= last_byte_mask[bset_tail_bits(nbits)];
}

size_t bset_num_zeroes(const bit_set * set)
{
	static const uint8_t num_zeroes[0x10] = {
		4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0,
	};

	uint8_t * bits = set->bits;
	size_t nbits = set->num_bits;
	size_t bytes = bset_bytes(nbits);
	size_t left_over_bits = bset_tail_bits(nbits);

	size_t acc = 0;
	uint8_t val = 0;
	for (size_t i = 0, end = bytes - (left_over_bits > 0); i < end; ++i)
	{
		val = bits[i];
		acc += num_zeroes[val & 0x0F] + num_zeroes[(val >> 4) & 0x0F];
	}

	if (left_over_bits)
	{
		val = bits[bytes-1];
		for (size_t i = 0; i < left_over_bits; ++i)
			acc += !((val >> i) & 1);
	}
	
	return acc;
}
