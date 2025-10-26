#include "../utils/global_defs.h"
#include "bit_wise.h"

namespace hv
{

	int bit_test(int bit_number, unsigned long* addr)
	{
		if (!addr)
		{
			return 0;
		}
		return (BITMAP_ENTRY(bit_number, addr) >> BITMAP_SHIFT(bit_number)) & 1;
	}

	void bit_clear(int bit_number, unsigned long* addr)
	{
		BITMAP_ENTRY(bit_number, addr) &= ~(1UL << BITMAP_SHIFT(bit_number));
	}

	void bit_set(int bit_number, unsigned long* addr)
	{
		BITMAP_ENTRY(bit_number, addr) |= (1UL << BITMAP_SHIFT(bit_number));
	}
}