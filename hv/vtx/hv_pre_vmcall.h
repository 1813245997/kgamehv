#pragma once

namespace hv
{
	namespace prevmcall
	{ 

		bool test_vmcall();

		void vmoff();
		
		void install_ept_hook( uint64_t pfn_of_hooked_page, uint64_t pfn_of_fake_page );

		void remove_ept_hook(uint64_t pfn_of_hooked_page);

		cr3  query_process_cr3(uint64_t pid);

		size_t read_virt_mem(cr3 cr3, void* dst, void const* src, size_t size);

		size_t write_virt_mem(cr3 cr3, void* dst, void const* src, size_t size);
	}
}