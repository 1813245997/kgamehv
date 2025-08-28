#pragma once

#include  "../ia32/ia32.hpp"
#include "msr_struct.h"
namespace hv {

struct mtrr_data {

  unsigned long long MaxPhysAddrBits;
  unsigned long long PhysAddrMask;
  ia32_vmx_ept_vpid_cap_register etp_vpid_cap;
  ia32_mtrr_capabilities_register cap;
  
  ia32_mtrr_def_type_register def_type;

  // fixed-range MTRRs
  union
  {
	  MTRR_FIXED_GENERIC Generic[11];
	  struct {
		  // 512-Kbyte range:
		  IA32_MTRR_FIX64K RangeFrom00000To7FFFF;

		  // Two 128-Kbyte ranges:
		  IA32_MTRR_FIX16K RangeFrom80000To9FFFF;
		  IA32_MTRR_FIX16K RangeFromA0000ToBFFFF;

		  // Eight 32-Kbyte ranges:
		  IA32_MTRR_FIX4K RangeFromC0000ToC7FFF;
		  IA32_MTRR_FIX4K RangeFromC8000ToCFFFF;
		  IA32_MTRR_FIX4K RangeFromD0000ToD7FFF;
		  IA32_MTRR_FIX4K RangeFromD8000ToDFFFF;
		  IA32_MTRR_FIX4K RangeFromE0000ToE7FFF;
		  IA32_MTRR_FIX4K RangeFromE8000ToEFFFF;
		  IA32_MTRR_FIX4K RangeFromF0000ToF7FFF;
		  IA32_MTRR_FIX4K RangeFromF8000ToFFFFF;
	  } Ranges;
  } Fixed;

  // variable-range MTRRs
  struct {
    ia32_mtrr_physbase_register base;
    ia32_mtrr_physmask_register mask;
  } variable[64];

  // number of valid variable-range MTRRs
  size_t var_count;

  bool IsSupported;
};

// read MTRR data into a single structure
mtrr_data read_mtrr_data();

// calculate the MTRR memory type for the given physical memory range
uint8_t calc_mtrr_mem_type(mtrr_data const& mtrrs, uint64_t address, uint64_t size);

} // namespace hv

