#pragma once

namespace hv
{
	namespace prevmcall
	{ 

		bool test_vmcall();

		void vmoff();
		
		void install_ept_hook( uint64_t pfn_of_hooked_page, uint64_t pfn_of_fake_page );

		void remove_ept_hook(uint64_t pfn_of_hooked_page);
	}
}