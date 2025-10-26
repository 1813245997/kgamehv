#pragma once
namespace hv
{
	int
		bit_test(int bit_number, unsigned long* addr);

	void
		bit_clear(int bit_number, unsigned long* addr);

	void
		bit_set(int bit_number, unsigned long* addr);

}

