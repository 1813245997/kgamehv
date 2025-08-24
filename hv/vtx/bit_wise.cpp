#include "../utils/global_defs.h"
#include "bit_wise.h"

/**
 * @brief Check whether the bit is set or not
 *
 * @param BitNumber
 * @param addr
 * @return int
 */
int
TestBit(int BitNumber, unsigned long* addr)
{
	if (!addr)
	{
		return 0;
	}
	return (BITMAP_ENTRY(BitNumber, addr) >> BITMAP_SHIFT(BitNumber)) & 1;
}

/**
 * @brief unset the bit
 *
 * @param BitNumber
 * @param addr
 */
void
ClearBit(int BitNumber, unsigned long* addr)
{
	BITMAP_ENTRY(BitNumber, addr) &= ~(1UL << BITMAP_SHIFT(BitNumber));
}

/**
 * @brief set the bit
 *
 * @param BitNumber
 * @param addr
 */
void
SetBit(int BitNumber, unsigned long* addr)
{
	BITMAP_ENTRY(BitNumber, addr) |= (1UL << BITMAP_SHIFT(BitNumber));
}