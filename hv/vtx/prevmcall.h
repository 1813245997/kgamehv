#pragma once
#include "../ia32/ia32.hpp"
namespace prevmcall
{
	size_t read_phys_mem(void* const dst,uint64_t const src, size_t const size);

	size_t write_phys_mem(uint64_t const dst,void const* const src, size_t const size);

	size_t read_virt_mem(cr3 const cr3, void* const dst,
		void const* const src, size_t const size);

	size_t write_virt_mem(cr3 const cr3, void* const dst,
		void const* const src, size_t const size);

	void install_ept_hook(uint64_t original_pfn, uint64_t fake_pfn);
	 
	void remove_ept_hook(uint64_t pfn);

	cr3 query_process_cr3(uint64_t const pid);
}