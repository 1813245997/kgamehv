#pragma once

union __msr
{
	unsigned __int64 all;
	struct
	{
		unsigned __int32 low;
		unsigned __int32 high;
	};
};

union IA32_MTRR_FIX64K {
	unsigned long long Value;
	struct { // Maps the 512-Kbyte address range (0..7FFFF) divided into eight 64-Kbyte sub-ranges:
		unsigned long long TypeOf64KbRange0 : 8; // 00000..0FFFF
		unsigned long long TypeOf64KbRange1 : 8; // 10000..1FFFF
		unsigned long long TypeOf64KbRange2 : 8; // 20000..2FFFF
		unsigned long long TypeOf64KbRange3 : 8; // 30000..3FFFF
		unsigned long long TypeOf64KbRange4 : 8; // 40000..4FFFF
		unsigned long long TypeOf64KbRange5 : 8; // 50000..5FFFF
		unsigned long long TypeOf64KbRange6 : 8; // 60000..6FFFF
		unsigned long long TypeOf64KbRange7 : 8; // 70000..7FFFF
	} Bitmap, Range00000;
};
static_assert(sizeof(IA32_MTRR_FIX64K) == sizeof(unsigned long long), "Size of IA32_MTRR_FIX64K != sizeof(unsigned long long)");

union IA32_MTRR_FIX16K {
	unsigned long long Value;
	struct {
		unsigned long long TypeOf16KbRange0 : 8; // 80000..83FFF  |  A0000..A3FFF
		unsigned long long TypeOf16KbRange1 : 8; // 84000..87FFF  |  A4000..A7FFF
		unsigned long long TypeOf16KbRange2 : 8; // 88000..8BFFF  |  A8000..ABFFF
		unsigned long long TypeOf16KbRange3 : 8; // 8C000..8FFFF  |  AC000..AFFFF
		unsigned long long TypeOf16KbRange4 : 8; // 90000..93FFF  |  B0000..B3FFF
		unsigned long long TypeOf16KbRange5 : 8; // 94000..97FFF  |  B4000..B7FFF
		unsigned long long TypeOf16KbRange6 : 8; // 98000..9BFFF  |  B8000..BBFFF
		unsigned long long TypeOf16KbRange7 : 8; // 9C000..9FFFF  |  BC000..BFFFF
	} Bitmap, Range80000, RangeA0000;
};
static_assert(sizeof(IA32_MTRR_FIX16K) == sizeof(unsigned long long), "Size of IA32_MTRR_FIX16K != sizeof(unsigned long long)");


union IA32_MTRR_FIX4K {
	unsigned long long Value;
	struct {
		unsigned long long TypeOf4KbRange0 : 8; // [C..F]0000..[C..F]0FFF  |  [C..F]8000..[C..F]8FFF
		unsigned long long TypeOf4KbRange1 : 8; // [C..F]1000..[C..F]1FFF  |  [C..F]9000..[C..F]9FFF
		unsigned long long TypeOf4KbRange2 : 8; // [C..F]2000..[C..F]2FFF  |  [C..F]A000..[C..F]AFFF
		unsigned long long TypeOf4KbRange3 : 8; // [C..F]3000..[C..F]3FFF  |  [C..F]B000..[C..F]BFFF
		unsigned long long TypeOf4KbRange4 : 8; // [C..F]4000..[C..F]4FFF  |  [C..F]C000..[C..F]CFFF
		unsigned long long TypeOf4KbRange5 : 8; // [C..F]5000..[C..F]5FFF  |  [C..F]D000..[C..F]DFFF
		unsigned long long TypeOf4KbRange6 : 8; // [C..F]6000..[C..F]6FFF  |  [C..F]E000..[C..F]EFFF
		unsigned long long TypeOf4KbRange7 : 8; // [C..F]7000..[C..F]7FFF  |  [C..F]F000..[C..F]FFFF
	} Bitmap,
		RangeC0000, RangeC8000,
		RangeD0000, RangeD8000,
		RangeE0000, RangeE8000,
		RangeF0000, RangeF8000;
};
static_assert(sizeof(IA32_MTRR_FIX4K) == sizeof(unsigned long long), "Size of IA32_MTRR_FIX4K != sizeof(unsigned long long)");

union MTRR_FIXED_GENERIC {
	unsigned long long Value;
	struct {
		unsigned long long Range0 : 8;
		unsigned long long Range1 : 8;
		unsigned long long Range2 : 8;
		unsigned long long Range3 : 8;
		unsigned long long Range4 : 8;
		unsigned long long Range5 : 8;
		unsigned long long Range6 : 8;
		unsigned long long Range7 : 8;
	} Generic;
	IA32_MTRR_FIX64K Fix64k;
	IA32_MTRR_FIX16K Fix16k;
	IA32_MTRR_FIX4K  Fix4k;
};
static_assert(sizeof(MTRR_FIXED_GENERIC) == sizeof(unsigned long long), "Size of MTRR_FIXED_GENERIC != sizeof(unsigned long long)");
