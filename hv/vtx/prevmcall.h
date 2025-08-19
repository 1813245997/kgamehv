#pragma once
#include "../ia32/ia32.hpp"
namespace prevmcall
{
	void install_ept_hook(uint64_t original_pfn, uint64_t fake_pfn);
	 
	void remove_ept_hook(uint64_t pfn);

	cr3 query_process_cr3(uint64_t const pid);
}